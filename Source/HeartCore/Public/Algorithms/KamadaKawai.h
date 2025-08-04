// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Nodesoup.h"
#include "Containers/Array.h"

namespace Nodesoup
{
    // https://gist.github.com/terakun/b7eff90c889c1485898ec9256ca9f91d
    class KamadaKawai
    {
    public:
        KamadaKawai(FGraphView InGraph, double Strength = 300.0, double InEnergyThreshold = 1e-2);

        void operator()(TArray<FVector2D>& Positions) const;

    private:
        struct FSpring
        {
            double Length = 0.0;
            double Strength = 0.0;
        };

        FGraphView Graph;
        const double EnergyThreshold;
        TArray<TArray<FSpring>> Springs;

        static FTwoDimIntArray FloydWarshall(FGraphView Graph);

        double FindMaxVertexEnergy(TConstArrayView<FVector2D> Positions, int32& MaxEnergyVertex) const;

        double ComputeVertexEnergy(int32 Vertex, TConstArrayView<FVector2D> Positions) const;

        FVector2D ComputeNextVertexPosition(int32 Vertex, TConstArrayView<FVector2D> Positions) const;
    };
}
