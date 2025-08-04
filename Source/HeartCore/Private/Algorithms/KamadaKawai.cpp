// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Algorithms/KamadaKawai.h"
#include "Containers/ArrayView.h"
#include "Math/Vector2D.h"

namespace Nodesoup
{
    KamadaKawai::KamadaKawai(FGraphView InGraph, const double Strength, const double InEnergyThreshold)
        : Graph(InGraph)
        , EnergyThreshold(InEnergyThreshold)
    {
        const FTwoDimIntArray Distances = FloydWarshall(Graph);

        // find the biggest distance
        size_t BiggestDistance = 0;
        for (int32 i = 0; i < Graph.Num(); i++)
        {
            for (int32 j = 0; j < Graph.Num(); j++)
            {
                if (Distances[i][j] > BiggestDistance)
                {
                    BiggestDistance = Distances[i][j];
                }
            }
        }

        // Ideal length for all edges. We don't really care, the layout is going to be scaled.
        // Let's choose 1.0 as the initial positions will be on a radius 1.0 circle, so we're
        // on the same order of magnitude
        const double Length = 1.0 / BiggestDistance;

        // init springs lengths and strengths matrices
        for (int32 i = 0; i < Graph.Num(); i++)
        {
            TArray<FSpring> VertexSprings;

            for (int32 j = 0; j < Graph.Num(); j++)
            {
                FSpring& Spring = VertexSprings.AddDefaulted_GetRef();
                if (i != j)
                {
                    const size_t Distance = Distances[i][j];
                    Spring.Length = Distance * Length;
                    Spring.Strength = Strength / (Distance * Distance);
                }
            }
            Springs.Add(VertexSprings);
        }
    }

    FTwoDimIntArray KamadaKawai::FloydWarshall(FGraphView Graph)
    {
        // build adjacency matrix (infinity = no edge, 1 = edge)
        constexpr uint32 Infinity = TNumericLimits<uint32>::Max() / 2;
        TArray<int32> InnerArray;
        FTwoDimIntArray Distances;
        InnerArray.Init(Infinity, Graph.Num());
        Distances.Init(InnerArray, Graph.Num());

        for (int32 Vertex = 0; Vertex < Graph.Num(); Vertex++)
        {
            Distances[Vertex][Vertex] = 0;
            for (const int32 Adjacent : Graph[Vertex])
            {
                if (Adjacent > Vertex)
                {
                    Distances[Vertex][Adjacent] = 1;
                    Distances[Adjacent][Vertex] = 1;
                }
            }
        }

        // floyd warshall itself, find length of the shortest path for each pair of vertices
        for (int32 k = 0; k < Graph.Num(); k++)
        {
            for (int32 i = 0; i < Graph.Num(); i++)
            {
                for (int32 j = 0; j < Graph.Num(); j++)
                {
                    Distances[i][j] = FMath::Min(Distances[i][j], Distances[i][k] + Distances[k][j]);
                }
            }
        }

        return Distances;
    }

    #define MAX_VERTEX_ITERS_COUNT 50
    #define MAX_STEADY_ENERGY_ITERS_COUNT 50

    /**
    Reduce the energy of the next vertex with most energy until all the vertices have
    an energy below energy_threshold
    */
    void KamadaKawai::operator()(TArray<FVector2D>& Positions) const
    {
        int32 Vertex;
        uint32 SteadyEnergyCount = 0;
        double MaxVertexEnergy = FindMaxVertexEnergy(Positions, Vertex);

        while (MaxVertexEnergy > EnergyThreshold && SteadyEnergyCount < MAX_STEADY_ENERGY_ITERS_COUNT)
        {
            // move vertex step by step until its energy goes below the threshold
            // (apparently this is equivalent to the newton raphson method)
            uint32 VertexCount = 0;
            do
            {
                Positions[Vertex] = ComputeNextVertexPosition(Vertex, Positions);
                VertexCount++;
            } while (ComputeVertexEnergy(Vertex, Positions) > EnergyThreshold && VertexCount < MAX_VERTEX_ITERS_COUNT);

            const double MaxVertexEnergyPrev = MaxVertexEnergy;
            MaxVertexEnergy = FindMaxVertexEnergy(Positions, Vertex);
            if (FMath::Abs(MaxVertexEnergy - MaxVertexEnergyPrev) < 1e-20)
            {
                SteadyEnergyCount++;
            }
            else
            {
                SteadyEnergyCount = 0;
            }
        }
    }

    /**
    Find @p max_energy_v_id with the most potential energy and @return its energy
    // https://gist.github.com/terakun/b7eff90c889c1485898ec9256ca9f91d
    */
    double KamadaKawai::FindMaxVertexEnergy(const TConstArrayView<FVector2D> Positions, int32& MaxEnergyVertex) const
    {
        double MaxEnergy = -1.0;
        for (int32 i = 0; i < Graph.Num(); i++)
        {
            const double Energy = ComputeVertexEnergy(i, Positions);
            if (Energy > MaxEnergy)
            {
                MaxEnergyVertex = i;
                MaxEnergy = Energy;
            }
        }
        ensure(MaxEnergy != -1.0);
        return MaxEnergy;
    }

    /** @return the potential energies of springs between @p v_id and all other vertices */
    double KamadaKawai::ComputeVertexEnergy(const int32 Vertex, const TConstArrayView<FVector2D> Positions) const
    {
        FVector2D Energy = FVector2D::ZeroVector;

        for (int32 i = 0; i < Graph.Num(); i++)
        {
            if (Vertex == i)
            {
                continue;
            }

            const FVector2D Delta = Positions[Vertex] - Positions[i];
            const double Distance = Delta.Size();

            const FSpring& Spring = Springs[Vertex][i];
            Energy += Delta * (Spring.Strength * (1.0 - Spring.Length / Distance));
        }

        return Energy.Size();
    }

    /**
    @returns next position for @param Vertex reducing its potential energy, ie the energy in the whole graph
    caused by its position.
    The position's delta depends on K (TODO bigger K = faster?).
    This is the complicated part of the algorithm.
    */
    FVector2D KamadaKawai::ComputeNextVertexPosition(const int32 Vertex, const TConstArrayView<FVector2D> Positions) const
    {
        FVector2D Energy = FVector2D::ZeroVector;
        FVector2D XEnergy = FVector2D::ZeroVector;
        FVector2D YEnergy = FVector2D::ZeroVector;

        for (int32 i = 0; i < Graph.Num(); i++)
        {
            if (Vertex == i)
            {
                continue;
            }

            const FVector2D Delta = Positions[Vertex] - Positions[i];
            const double Distance = Delta.Size();
            const double CubedDistance = FMath::Cube(Distance);

            const FSpring& Spring = Springs[Vertex][i];

            Energy += Delta * (Spring.Strength * (1.0 - Spring.Length / Distance));
            XEnergy.Y += Spring.Strength * Spring.Length * Delta.X * Delta.Y / CubedDistance;
            XEnergy.X += Spring.Strength * (1.0 - Spring.Length * Delta.Y * Delta.Y / CubedDistance);
            YEnergy.Y += Spring.Strength * (1.0 - Spring.Length * Delta.X * Delta.X / CubedDistance);
        }
        YEnergy.X = XEnergy.Y;

        FVector2D Position = Positions[Vertex];
        const double Denom = XEnergy.X * YEnergy.Y - XEnergy.Y * YEnergy.X;
        Position.X += (XEnergy.Y * Energy.Y - YEnergy.Y * Energy.X) / Denom;
        Position.Y += (XEnergy.Y * Energy.X - XEnergy.X * Energy.Y) / Denom;

        return Position;
    }
}
