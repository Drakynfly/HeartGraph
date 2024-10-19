// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Algorithms/Nodesoup.h"
#include "Algorithms/FruchtermanReingold.h"
#include "Algorithms/KamadaKawai.h"
#include "Algorithms/Layout.h"

namespace Nodesoup
{
    TArray<FVector2D> fruchterman_reingold(
        FGraphView Graph,
        const FGraphIterationCallback& Callback,
        const uint32 Width,
        const uint32 Height,
        const uint32 Iterations,
        const double Strength)
    {
        TArray<FVector2D> Positions;
        Positions.SetNumZeroed(Graph.Num());
        // Initial layout on a circle
        Circle(Positions);

        FruchtermanReingold fr(Graph, Strength);

        for (uint32 i = 0; i < Iterations; i++)
        {
            fr(Positions);

            TArray<FVector2D> ScaledPositions = Positions;
            CenterAndScale(Width, Height, ScaledPositions);
            Callback(ScaledPositions, i);
        }

        CenterAndScale(Width, Height, Positions);
        return Positions;
    }

    TArray<FVector2D> kamada_kawai(
        FGraphView Graph,
        const uint32 Width,
        const uint32 Height,
        const double Strength,
        const double EnergyThreshold)
    {
        TArray<FVector2D> Positions;
        Positions.SetNumZeroed(Graph.Num());
        // Initial layout on a circle
        Circle(Positions);
        const KamadaKawai kk(Graph, Strength, EnergyThreshold);
        kk(Positions);
        CenterAndScale(Width, Height, Positions);

        return Positions;
    }

    TArray<double> SizeRadii(FGraphView Graph, const double MinRadius, const double Strength)
    {
        TArray<double> Radii;
        Radii.Reserve(Graph.Num());
        for (int32 i = 0; i < Graph.Num(); i++)
        {
            const double Delta = FMath::Log2(Strength * static_cast<double>(Graph[i].Num()) / Graph.Num());
            double Radius = MinRadius + FMath::Max(0.0, Delta);
            Radii.Add(Radius);
        }
        return Radii;
    }
}