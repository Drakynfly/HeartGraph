// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGuids.h"
#include "HeartGraphTypes.generated.h"

class UHeartGraphNode;


USTRUCT(BlueprintType)
struct FHeartGraphConnectionEvent
{
	GENERATED_BODY()

	// @todo REFACTOR this should be FHeartNodeGuids
	UPROPERTY(BlueprintReadOnly, Category = "HeartGraphConnectionEvent")
	TSet<TObjectPtr<UHeartGraphNode>> AffectedNodes;

	UPROPERTY(BlueprintReadOnly, Category = "HeartGraphConnectionEvent")
	TSet<FHeartPinGuid> AffectedPins;
};

/* Event broadcast after nodes are added to a graph */
USTRUCT(BlueprintType)
struct FHeartNodeAddEvent
{
	GENERATED_BODY()

	// Nodes that were added. Usually just one.
	// @todo Implement Batch node addition
	UPROPERTY(BlueprintReadOnly, Category = "HeartNodeAddEvent")
	TArray<FHeartNodeGuid> NewNodes;
};

USTRUCT(BlueprintType)
struct FHeartNodeRemoveEvent
{
	GENERATED_BODY()

	// Nodes that were removed. Usually just one.
	// @todo Implement Batch node removal
	UPROPERTY(BlueprintReadOnly, Category = "HeartNodeRemoveEvent")
	TArray<TObjectPtr<UHeartGraphNode>> AffectedNodes;
};

USTRUCT(BlueprintType)
struct FHeartNodeMoveEvent
{
	GENERATED_BODY()

	// Nodes being moved. Usually just one, but mass node movement might be enabled in graph using a marquee tool.
	// @todo REFACTOR this should be FHeartNodeGuids
	UPROPERTY(BlueprintReadOnly, Category = "HeartNodeMoveEvent")
	TSet<TObjectPtr<UHeartGraphNode>> AffectedNodes;

	// Is the move "in-progress" or finished, typically during drag-drop style movement.
	UPROPERTY(BlueprintReadOnly, Category = "HeartNodeMoveEvent")
	bool MoveFinished = false;
};

USTRUCT(BlueprintType)
struct FHeartGraphNodeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "HeartGraphNodeMessage")
	FText Message;

	UPROPERTY(BlueprintReadOnly, Category = "HeartGraphNodeMessage")
	FLinearColor Color = FLinearColor::White;
};