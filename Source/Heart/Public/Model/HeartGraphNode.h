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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNodeRefreshRequested, UHeartGraphNode*, Node);

USTRUCT()
struct FHeartGraphNodeSparseClassData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Pins")
	TArray<TObjectPtr<UHeartGraphPin>> DefaultInputs;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Pins")
	TArray<TObjectPtr<UHeartGraphPin>> DefaultOutputs;
};

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable, SparseClassDataTypes = "HeartGraphNodeSparseClassData")
class HEART_API UHeartGraphNode : public UObject
{
	GENERATED_BODY()

	friend class UHeartGraph;
	friend class UHeartGraphSchema;

public:
	virtual UWorld* GetWorld() const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	UClass* GetSupportedClass() const;


	/****************************/
	/**		REFLECTION			*/
	/****************************/

	/** Returns the name of this node. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetNodeTitle() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetNodeCategory() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetNodeToolTip() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool GetDynamicTitleColor(FLinearColor& LinearColor);


	/****************************/
	/**		GETTERS				*/
	/****************************/

#if WITH_EDITOR
	UEdGraphNode* GetEdGraphNode() const { return HeartEdGraphNode; }
#endif

	template <typename TNodeClass>
	TNodeClass* GetNodeObject() const
	{
		return Cast<TNodeClass>(NodeObject);
	}

	UFUNCTION(BlueprintCallable, Category = "Node|GraphNode")
	UObject* GetNodeObject() const { return NodeObject; }

	UFUNCTION(BlueprintCallable, Category = "Node|GraphNode", meta = (DeterminesOutputType = Class))
	UObject* GetNodeObjectTyped(TSubclassOf<UObject> Class) const { return NodeObject; }

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

	template <typename THeartGraphPin = UHeartGraphPin>
	TArray<THeartGraphPin*> GetPinsOfDirection(EHeartPinDirection Direction) const
	{
		static_assert(TIsDerivedFrom<THeartGraphPin, UHeartGraphPin>::IsDerived, "The pin class must derive from UHeartGraphPin");
		auto&& DirectedPins = GetPinsOfDirection(Direction, THeartGraphPin::StaticClass());
		return *reinterpret_cast<TArray<THeartGraphPin*>*>(&DirectedPins);
	}

	template <typename THeartGraphPin = UHeartGraphPin>
	TArray<THeartGraphPin*> GetInputPins() const { return GetPinsOfDirection<THeartGraphPin>(EHeartPinDirection::Input); }

	template <typename THeartGraphPin = UHeartGraphPin>
	TArray<THeartGraphPin*> GetOutputPins() const { return GetPinsOfDirection<THeartGraphPin>(EHeartPinDirection::Output); }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	TArray<UHeartGraphPin*> GetPinsOfDirection(EHeartPinDirection Direction, TSubclassOf<UHeartGraphPin> Class) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	TArray<UHeartGraphPin*> GetInputPins(TSubclassOf<UHeartGraphPin> Class) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	TArray<UHeartGraphPin*> GetOutputPins(TSubclassOf<UHeartGraphPin> Class) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	uint8 GetUserInputNum() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	uint8 GetUserOutputNum() const;

	// Get the input pins dynamically generated by the node's state
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure = false, Category = "Heart|GraphNode")
	TArray<UHeartGraphPin*> GetDynamicInputs() const;

	// Get the input pins dynamically generated by the node's state
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure = false, Category = "Heart|GraphNode")
	TArray<UHeartGraphPin*> GetDynamicOutputs() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure = false, Category = "Heart|GraphNode")
	UHeartGraphPin* GetInstancedPin();

	/****************************/
	/**		NODE EDITING		*/
	/****************************/
public:
#if WITH_EDITOR
	void SetEdGraphNode(UEdGraphNode* GraphNode);
#endif

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	void SetLocation(const FVector2D& NewLocation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool SupportsDynamicPins();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanUserAddInput();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanUserAddOutput();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanDelete() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanDuplicate() const;


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
	THeartGraphPin* CreatePin(const TSubclassOf<UHeartGraphPin> Class, FName Name, const EHeartPinDirection Direction)
	{
		static_assert(TIsDerivedFrom<THeartGraphPin, UHeartGraphPin>::IsDerived, "The pin class must derive from UHeartGraphPin");
		check(Class->IsChildOf<THeartGraphPin>());
		return Cast<THeartGraphPin>(CreatePin(Class, Name, Direction));
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	UHeartGraphPin* CreatePin(TSubclassOf<UHeartGraphPin> Class, FName Name, EHeartPinDirection Direction);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	void AddPin(UHeartGraphPin* Pin);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	bool RemovePin(UHeartGraphPin* Pin);

	// @todo refactor to not use FName. either use UHeartGraphPin* or FHeartPinGuid
	void RemoveUserInput(const FName& PinName);
	void RemoveUserOutput(const FName& PinName);

public:
	virtual void NotifyPinConnectionsChanged(UHeartGraphPin* Pin);

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPinConnectionsChanged OnPinConnectionsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGraphNodePinChanged OnNodePinsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGraphNodeLocationChanged OnNodeLocationChanged;

	UPROPERTY(BlueprintCallable, Category = "Events")
	FOnNodeRefreshRequested OnReconstructionRequested;

protected:
	// The object that this graph node represents. Contains the data and functionality of a spawned instance.
	UPROPERTY()
	TObjectPtr<UObject> NodeObject;

	UPROPERTY()
	FHeartNodeGuid Guid;

	UPROPERTY()
	FVector2D Location;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<FHeartPinGuid, TObjectPtr<UHeartGraphPin>> Pins;

private:
#if WITH_EDITORONLY_DATA
	// Always castable to UHeartEdGraphNode
	UPROPERTY()
	TObjectPtr<UEdGraphNode> HeartEdGraphNode;
#endif
};