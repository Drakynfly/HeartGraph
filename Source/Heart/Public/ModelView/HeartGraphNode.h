// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Model/HeartGuids.h"
#include "HeartGraphNode.generated.h"

class UHeartGraph;
class UHeartGraphPin;
class UHeartGraphCanvas;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPinConnectionsChanged, UHeartGraphPin*, Pin);

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class HEART_API UHeartGraphNode : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

	void NewGuid();

	template <typename TUHeartGraphClass>
	TUHeartGraphClass* GetOwningGraph() const
	{
		return Cast<TUHeartGraphClass>(GetOuter());
	}

	virtual void NotifyPinConnectionsChanged(UHeartGraphPin* Pin);

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraph* GetGraph() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = "Class"))
	UHeartGraph* GetGraphTyped(TSubclassOf<UHeartGraph> Class) const { return GetGraph(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FHeartNodeGuid GetGuid() const { return Guid; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	UClass* GetSupportedClass() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FVector2D GetLocation() const { return Location; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	void SetLocation(const FVector2D& NewLocation);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraphPin* GetPin(const FHeartPinGuid& PinGuid);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	void AddPin(UHeartGraphPin* Pin);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	bool RemovePin(UHeartGraphPin* Pin);

public:
	UPROPERTY(BlueprintAssignable, Category = "Heart|GraphNode|Events")
	FOnPinConnectionsChanged OnPinConnectionsChanged;

protected:
	UPROPERTY()
	FHeartNodeGuid Guid;

	UPROPERTY()
	FVector2D Location;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<FHeartPinGuid, TObjectPtr<UHeartGraphPin>> Pins;
};