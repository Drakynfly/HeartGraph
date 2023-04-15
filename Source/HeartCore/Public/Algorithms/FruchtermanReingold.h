// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Nodesoup.h"

namespace Nodesoup
{
    class FruchtermanReingold
    {
    public:
        FruchtermanReingold(FGraphView InGraph, double InStrength = 15.0);

        void operator()(TArray<FVector2D>& Positions);

    private:
        const FGraphView Graph;
        const double Strength;
        const double StrengthSqr;
        double Temp;
        TArray<FVector2D> Movements;
    };
}
