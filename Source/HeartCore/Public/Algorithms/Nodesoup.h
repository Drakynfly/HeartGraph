// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

namespace Nodesoup
{
    /** Simple adjacency list graph structure */

    using FTwoDimIntArray = TArray<TArray<int32>>;

    using FGraphView = const FTwoDimIntArray&;

    /** Main library functions */

    using FGraphIterationCallback = TFunctionRef<void(TConstArrayView<FVector2D>, int32)>;

    /**
     * Applies the Freuchterman-Reingold algorithm to layout graph @p in a frame of dimensions
     * @p width and @p height, in @p iter-count iterations
     */
    HEARTCORE_API TArray<FVector2D> fruchterman_reingold(
        FGraphView Graph,
        const FGraphIterationCallback& Callback,
        uint32 Width,
        uint32 Height,
        uint32 Iterations = 300,
        double Strength = 15.0);

    HEARTCORE_API TArray<FVector2D> kamada_kawai(
        FGraphView Graph,
        uint32 Width,
        uint32 Height,
        double Strength = 300.0,
        double EnergyThreshold = 1e-2);

    /** Assigns diameters to vertices based on their degree */
    HEARTCORE_API TArray<double> SizeRadii(FGraphView Graph, double MinRadius = 4.0, double Strength = 300.0);
}