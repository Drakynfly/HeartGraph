// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Algorithms/FruchtermanReingold.h"

namespace Nodesoup
{
    FruchtermanReingold::FruchtermanReingold(FGraphView InGraph, const double InStrength)
        : Graph(InGraph)
        , Strength(InStrength)
        , StrengthSqr(Strength * Strength)
        , Temperature(10 * FMath::Sqrt(static_cast<double>(Graph.Num())))
    {
        Movements.SetNumZeroed(Graph.Num());
    }

    void FruchtermanReingold::operator()(TArray<FVector2D>& Positions)
    {
        for (int32 Node = 0; Node < Graph.Num(); Node++)
        {
            // Repulsion force between vertex pairs
            for (int32 Adj = Node + 1; Adj < Graph.Num(); Adj++)
            {
                if (Node == Adj)
                {
                    continue;
                }

                FVector2D Delta = Positions[Node] - Positions[Adj];
                const double Distance = Delta.Size();
                // TODO: handle distance == 0.0

                // > 1000.0: not worth computing
                if (Distance > 1000.0)
                {
                    continue;
                }

                const double Repulsion = StrengthSqr / Distance;

                const FVector2D Movement = Delta / Distance * Repulsion;
                Movements[Node] += Movement;
                Movements[Adj] -= Movement;
            }

            // Attraction force between edges
            for (const int32 Adj : Graph[Node])
            {
                if (Adj > Node)
                {
                    continue;
                }

                FVector2D Delta = Positions[Node] - Positions[Adj];
                const double Distance = Delta.Size();
                if (FMath::IsNearlyZero(Distance))
                {
                    continue;
                }

                const double Attraction = Distance * Distance / Strength;

                const FVector2D Movement = Delta / Distance * Attraction;
                Movements[Node] -= Movement;
                Movements[Adj] += Movement;
            }
        }

        // Max movement capped by current temperature
        for (int32 Node = 0; Node < Graph.Num(); Node++)
        {
            const double MovementSize = Movements[Node].Size();
            // < 1.0: not worth computing
            if (MovementSize < 1.0)
            {
                continue;
            }
            const double CappedMovementSize = FMath::Min(MovementSize, Temperature);
            const FVector2D CappedMovement = Movements[Node] / MovementSize * CappedMovementSize;

            Positions[Node] += CappedMovement;
        }

        // Cool down fast until we reach 1.5, then stay at low temperature
        if (Temperature > 1.5)
        {
            Temperature *= 0.85;
        }
        else
        {
            Temperature = 1.5;
        }
    }
}