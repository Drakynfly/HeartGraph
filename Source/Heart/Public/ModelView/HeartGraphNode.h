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
	UFUNCTION(BlueprintCallable, Category = "HeartGraphNode")
	UHeartGraph* GetGraph() const;

	UFUNCTION(BlueprintCallable, Category = "HeartGraphNode", meta = (DeterminesOutputType = "Class"))
	UHeartGraph* GetGraphTyped(TSubclassOf<UHeartGraph> Class) const { return GetGraph(); }

	UFUNCTION(BlueprintCallable, Category = "HeartGraphNode")
	FHeartNodeGuid GetGuid() const { return Guid; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "HeartGraphNode")
	UClass* GetSupportedClass() const;

	UFUNCTION(BlueprintCallable, Category = "HeartGraphNode")
	FVector2D GetLocation() const { return FVector2D(Location); }

	UFUNCTION(BlueprintCallable, Category = "HeartGraphNode")
	void SetLocation(const FVector2D& NewLocation);

	UFUNCTION(BlueprintCallable, Category = "HeartGraphNode")
	UHeartGraphPin* GetPin(const FHeartPinGuid& PinGuid);

	UFUNCTION(BlueprintCallable, Category = "HeartGraphNode")
	void AddPin(UHeartGraphPin* Pin);

	UFUNCTION(BlueprintCallable, Category = "HeartGraphNode")
	bool RemovePin(UHeartGraphPin* Pin);

public:
	UPROPERTY(BlueprintAssignable, Category = "HeartGraphNode|Events")
	FOnPinConnectionsChanged OnPinConnectionsChanged;

protected:
	UPROPERTY()
	FHeartNodeGuid Guid;

	UPROPERTY()
	FVector2D Location;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<FHeartPinGuid, TObjectPtr<UHeartGraphPin>> Pins;
};