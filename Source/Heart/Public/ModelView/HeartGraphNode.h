// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Model/HeartGuids.h"
#include "Model/HeartPinDirection.h"
#include "HeartGraphNode.generated.h"

class UHeartGraph;
class UHeartGraphPin;
class UHeartGraphCanvas;

class UHeartGraphNode;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPinConnectionsChanged, UHeartGraphPin*, Pin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGraphNodePinChanged, UHeartGraphNode*, Node);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGraphNodeLocationChanged, UHeartGraphNode*, Node, const FVector2D&, Location);

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class HEART_API UHeartGraphNode : public UObject
{
	GENERATED_BODY()

	friend class UHeartGraph;
	friend class UHeartGraphSchema;

public:
	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	UClass* GetSupportedClass() const;


	/****************************/
	/**		GETTERS				*/
	/****************************/

	template <typename THeartGraph>
	THeartGraph* GetOwningGraph() const
	{
		static_assert(TIsDerivedFrom<THeartGraph, UHeartGraph>::IsDerived, "The graph class must derive from UHeartGraph");
		return Cast<THeartGraph>(GetOuter());
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraph* GetGraph() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = "Class"))
	UHeartGraph* GetGraphTyped(TSubclassOf<UHeartGraph> Class) const { return GetGraph(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FHeartNodeGuid GetGuid() const { return Guid; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FVector2D GetLocation() const { return Location; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraphPin* GetPin(const FHeartPinGuid& PinGuid);

	template <typename THeartGraphPin>
	TArray<THeartGraphPin*> GetPinsOfDirection(EHeartPinDirection Direction) const
	{
		static_assert(TIsDerivedFrom<THeartGraphPin, UHeartGraphPin>::IsDerived, "The pin class must derive from UHeartGraphPin");
		auto&& DirectedPins = GetPinsOfDirection(Direction, THeartGraphPin::StaticClass());
		return *reinterpret_cast<TArray<THeartGraphPin*>*>(&DirectedPins);
	}

	template <typename THeartGraphPin>
	TArray<THeartGraphPin*> GetInputPins() const { return GetPinsOfDirection<THeartGraphPin>(EHeartPinDirection::Input); }

	template <typename THeartGraphPin>
	TArray<THeartGraphPin*> GetOutputPins() const { return GetPinsOfDirection<THeartGraphPin>(EHeartPinDirection::Output); }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	TArray<UHeartGraphPin*> GetPinsOfDirection(EHeartPinDirection Direction, TSubclassOf<UHeartGraphPin> Class) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	TArray<UHeartGraphPin*> GetInputPins(TSubclassOf<UHeartGraphPin> Class) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	TArray<UHeartGraphPin*> GetOutputPins(TSubclassOf<UHeartGraphPin> Class) const;


	/****************************/
	/**		NODE EDITING		*/
	/****************************/
public:
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	void SetLocation(const FVector2D& NewLocation);


	/****************************/
	/**		PIN EDITING			*/
	/****************************/

	template <typename THeartGraphPin>
	THeartGraphPin* CreatePin(EHeartPinDirection Direction)
	{
		static_assert(TIsDerivedFrom<THeartGraphPin, UHeartGraphPin>::IsDerived, "The pin class must derive from UHeartGraphPin");
		return Cast<THeartGraphPin>(CreatePin(THeartGraphPin::StaticClass(), Direction));
	}

	template <typename THeartGraphPin>
	THeartGraphPin* CreatePin(const TSubclassOf<UHeartGraphPin> Class, const EHeartPinDirection Direction)
	{
		static_assert(TIsDerivedFrom<THeartGraphPin, UHeartGraphPin>::IsDerived, "The pin class must derive from UHeartGraphPin");
		check(Class->IsChildOf<THeartGraphPin>());
		return Cast<THeartGraphPin>(CreatePin(Class, Direction));
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	UHeartGraphPin* CreatePin(TSubclassOf<UHeartGraphPin> Class, EHeartPinDirection Direction);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	void AddPin(UHeartGraphPin* Pin);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	bool RemovePin(UHeartGraphPin* Pin);

public:
	virtual void NotifyPinConnectionsChanged(UHeartGraphPin* Pin);

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPinConnectionsChanged OnPinConnectionsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGraphNodePinChanged OnNodePinsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGraphNodeLocationChanged OnNodeLocationChanged;

protected:
	UPROPERTY()
	FHeartNodeGuid Guid;

	UPROPERTY()
	FVector2D Location;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<FHeartPinGuid, TObjectPtr<UHeartGraphPin>> Pins;
};