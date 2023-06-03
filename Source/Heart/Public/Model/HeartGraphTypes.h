// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGuids.h"
#include "HeartGraphTypes.generated.h"

class UHeartGraphNode;

USTRUCT(BlueprintType)
struct FHeartGraphConnectionEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "ConnectionEvent")
	TSet<TObjectPtr<UHeartGraphNode>> AffectedNodes;

	UPROPERTY(BlueprintReadOnly, Category = "ConnectionEvent")
	TSet<FHeartPinGuid> AffectedPins;
};

USTRUCT(BlueprintType)
struct FHeartNodeMoveEvent
{
	GENERATED_BODY()

	// Nodes being moved. Usually just one, but mass node movement might be enabled in graph using a marquee tool.
	UPROPERTY(BlueprintReadOnly, Category = "ConnectionEvent")
	TSet<TObjectPtr<UHeartGraphNode>> AffectedNodes;

	// Is the move "in-progress" or finished, typically during drag-drop style movement.
	UPROPERTY(BlueprintReadOnly, Category = "ConnectionEvent")
	bool MoveFinished;
};

USTRUCT(BlueprintType)
struct FHeartGraphNodeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "NodeMessage")
	FText Message;

	UPROPERTY(BlueprintReadOnly, Category = "NodeMessage")
	FLinearColor Color = FLinearColor::White;
};


