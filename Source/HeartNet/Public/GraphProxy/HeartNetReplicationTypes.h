// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartReplicateNodeData.h"
#include "Model/HeartGraphPinReference.h"
#include "Input/HeartInputActivation.h"
#include "HeartNetReplicationTypes.generated.h"

USTRUCT()
struct FHeartNodeMoveEvent_Net
{
	GENERATED_BODY()

	// Nodes being moved. Usually just one, but mass node movement might be enabled in graph using a marquee tool.
	UPROPERTY()
	TArray<FHeartNodeFlake> AffectedNodes;

	// Is the move "in-progress" or finished, typically during drag-drop style movement?
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

USTRUCT()
struct FHeartRemoteGraphActionArguments
{
	GENERATED_BODY()

	UPROPERTY()
	FHeartGraphPinReference NodeAndPinGuid;

	// Custom target for executing actions on pins.
	UPROPERTY()
	TObjectPtr<UObject> PinTarget;

	UPROPERTY()
	FHeartManualEvent Activation;

	// This object has to exist on the server for it to be replicated. Heart does not handle this automatically.
	UPROPERTY()
	TObjectPtr<UObject> ContextObject;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		uint8 HasValidNodeGuid = NodeAndPinGuid.NodeGuid.IsValid();
		uint8 HasValidPinGuid = NodeAndPinGuid.PinGuid.IsValid();

		if (Ar.IsSaving())
		{
			HasValidNodeGuid = NodeAndPinGuid.NodeGuid.IsValid();
			HasValidPinGuid = NodeAndPinGuid.PinGuid.IsValid();
		}

		// Serialize single bits for which guids are valid
		Ar.SerializeBits(&HasValidNodeGuid, 1);
		Ar.SerializeBits(&HasValidPinGuid, 1);

		if (HasValidNodeGuid)
		{
			Ar << NodeAndPinGuid.NodeGuid;
		}

		if (HasValidPinGuid)
		{
			Ar << NodeAndPinGuid.PinGuid;
		}

		Ar << PinTarget;
		Ar << Activation.EventValue;
		Ar << ContextObject;

		bOutSuccess &= !Ar.IsError();
		return bOutSuccess;
	}
};

template<>
struct TStructOpsTypeTraits<FHeartRemoteGraphActionArguments> : public TStructOpsTypeTraitsBase2<FHeartRemoteGraphActionArguments>
{
	enum
	{
		WithDeltaSerializer = true,
	};
};