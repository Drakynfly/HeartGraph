// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGraphPinTag.h"
#include "HeartGraphPinDesc.h"
#include "Model/HeartGuids.h"
#include "Model/HeartGraphPinReference.h"
#include "Model/HeartPinDirection.h"
#include "HeartGraphPin.generated.h"

class UHeartGraphNode;
class UHeartGraphPin;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeartPinConnectionsChanged, UHeartGraphPin*, Pin);

/**
 *
 */
UCLASS(BlueprintType)
class HEART_API UHeartGraphPin : public UObject // Based on UEdGraphPin
{
	GENERATED_BODY()

	friend UHeartGraphNode;
	friend class UHeartEdGraphNode;

public:
	bool ConnectTo(UHeartGraphPin* Other);

	void DisconnectFrom(const FHeartGraphPinReference Other, bool NotifyNode);

	void DisconnectFromAll(bool NotifyNodes);


	/*----------------------------
			REFLECTION
	----------------------------*/

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	EHeartPinDirection GetDirection() const;

	/*----------------------------
				GETTERS
	----------------------------*/
protected:
#if WITH_EDITOR
	// @todo this data needs to be exposed better. maybe not even made here, but using out data. what about UHeartEdGraphNode make the FEdGraphPinType
	FEdGraphPinType GetPinType() const;
#endif

public:
	template <typename THeartGraphNode>
	THeartGraphNode* GetOwningNode() const
	{
		static_assert(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::IsDerived, "The graph node class must derive from UHeartGraphNode");
		return Cast<THeartGraphNode>(GetOuter());
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	UHeartGraphNode* GetNode() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = "false",
				meta = (DeprecatedFunction, DeterminesOutputType = "Class", DynamicOutputParam = "Node", ExpandBoolAsExecs = "ReturnValue"))
	bool GetNodeTyped(TSubclassOf<UHeartGraphNode> Class, UHeartGraphNode*& Node) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	FHeartPinGuid GetGuid() const { return Guid; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	FHeartGraphPinReference GetReference() const;

	const TArray<FHeartGraphPinReference>& GetLinks() const { return Links; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	bool IsConnected() const { return !Links.IsEmpty(); }

	int32 GetNumLinks() const { return Links.Num(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	TArray<UHeartGraphPin*> GetAllConnections();

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphPin")
	UHeartGraphPin* ResolveConnectionByReference(const FHeartGraphPinReference Reference) const;

	/*
	 * Gets a connection pin for the given index. Treat this like an array, only access indices you know to be valid.
	 * Use GetNumLinks or IsConnected to determine this first.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphPin")
	UHeartGraphPin* ResolveConnection(const int32 Index) const;

public:
	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FHeartPinConnectionsChanged OnPinConnectionsChanged;

public:
	UPROPERTY(BlueprintReadOnly)
	FHeartPinGuid Guid;

	UPROPERTY(BlueprintReadOnly)
	FHeartGraphPinDesc PinDesc;

	UPROPERTY(BlueprintReadOnly)
	TArray<FHeartGraphPinReference> Links;
};
