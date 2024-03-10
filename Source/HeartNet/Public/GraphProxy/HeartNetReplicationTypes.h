// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartReplicateNodeData.h"
#include "Model/HeartGraphPinReference.h"
#include "HeartNetReplicationTypes.generated.h"

USTRUCT()
struct FHeartNodeMoveEvent_Net
{
	GENERATED_BODY()

	// Nodes being moved. Usually just one, but mass node movement might be enabled in graph using a marquee tool.
	UPROPERTY()
	TArray<FHeartNodeFlake> AffectedNodes;

	// Is the move "in-progress" or finished, typically during drag-drop style movement.
	UPROPERTY()
	bool MoveFinished = false;
};

USTRUCT()
struct FHeartGraphConnectionEvent_Net_PinElement
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FHeartPinGuid, FHeartGraphPinConnections> PinConnections;
};

USTRUCT()
struct FHeartGraphConnectionEvent_Net
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FHeartNodeFlake> AffectedNodes;
};