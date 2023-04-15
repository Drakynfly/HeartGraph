// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Algorithms/FruchtermanReingold.h"

namespace Nodesoup
{
    FruchtermanReingold::FruchtermanReingold(const FGraphView InGraph, const double InStrength)
        : Graph(InGraph)
        , Strength(InStrength)
        , StrengthSqr(Strength * Strength)
        , Temp(10 * sqrt(Graph.Num()))
    {
        Movements.SetNumZeroed(Graph.Num());
    }

    void FruchtermanReingold::operator()(TArray<FVector2D>& Positions)
    {
        // Repulsion force between vertex pairs
        for (int32 i = 0; i < Graph.Num(); i++)
        {
            for (int32 j = i + 1; j < Graph.Num(); j++)
            {
                if (i == j)
                {
                    continue;
                }

                FVector2D Delta = Positions[i] - Positions[j];
                const double Distance = Delta.Size();
                // TODO: handle distance == 0.0

                // > 1000.0: not worth computing
                if (Distance > 1000.0)
                {
                    continue;
                }

                const double Repulsion = StrengthSqr / Distance;

                Movements[i] += Delta / Distance * Repulsion;
                Movements[j] -= Delta / Distance * Repulsion;
            }

            // Attraction force between edges
            for (const int32 adj_id : Graph[i])
            {
                if (adj_id > i)
                {
                    continue;
                }

                FVector2D Delta = Positions[i] - Positions[adj_id];
                const double Distance = Delta.Size();
                if (Distance == 0.0)
                {
                    continue;
                }

                const double Attraction = Distance * Distance / Strength;

                Movements[i] -= Delta / Distance * Attraction;
                Movements[adj_id] += Delta / Distance * Attraction;
            }
        }

        // Max movement capped by current temperature
        for (int32 i = 0; i < Graph.Num(); i++)
        {
            double mvmt_norm = Movements[i].Size();
            // < 1.0: not worth computing
            if (mvmt_norm < 1.0)
            {
                continue;
            }
            const double capped_mvmt_norm = FMath::Min(mvmt_norm, Temp);
            const FVector2D capped_mvmt = Movements[i] / mvmt_norm * capped_mvmt_norm;

            Positions[i] += capped_mvmt;
        }

        // Cool down fast until we reach 1.5, then stay at low temperature
        if (Temp > 1.5)
        {
            Temp *= 0.85;
        }
        else
        {
            Temp = 1.5;
        }
    }
}
