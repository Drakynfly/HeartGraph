// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Nodesoup.h"
#include "Containers/Array.h"

namespace Nodesoup
{
    class HEARTCORE_API FruchtermanReingold
    {
    public:
        FruchtermanReingold(FGraphView InGraph, double InStrength = 15.0);

        void operator()(TArray<FVector2D>& Positions);

    private:
        FGraphView Graph;
        const double Strength;
        const double StrengthSqr;
        double Temperature;
        TArray<FVector2D> Movements;
    };
}
