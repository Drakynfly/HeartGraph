// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraph.h"
#include "HeartGraphPinType.h"
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

	UPROPERTY(EditDefaultsOnly, Category = "Pins")
	TMap<FName, FHeartGraphPinType> DefaultInputs;

	UPROPERTY(EditDefaultsOnly, Category = "Pins")
	TMap<FName, FHeartGraphPinType> DefaultOutputs;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Editor|Pins")
	bool SupportsDynamicPinsInEditor = false;
#endif
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

	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;

#if WITH_EDITOR
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	UClass* GetSupportedClass() const;


	/****************************/
	/**		REFLECTION			*/
	/****************************/

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetDefaultNodeTitle(const UObject* Node) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetDefaultNodeCategory(const UObject* Node) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetDefaultNodeToolTip(const UObject* Node) const;

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

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UObject* GetNodeObject() const { return NodeObject; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	UObject* GetNodeObjectTyped(TSubclassOf<UObject> Class) const { return NodeObject; }

	template <typename THeartGraph>
	THeartGraph* GetOwningGraph() const
	{
		static_assert(TIsDerivedFrom<THeartGraph, UHeartGraph>::IsDerived, "The graph class must derive from UHeartGraph");
		check(GetOuter()->IsA(UHeartGraph::StaticClass())); // This must always succeed, even if the cast below doesn't
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
	TMap<FName, FHeartGraphPinType> GetDynamicInputs() const;

	// Get the input pins dynamically generated by the node's state
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure = false, Category = "Heart|GraphNode")
	TMap<FName, FHeartGraphPinType> GetDynamicOutputs() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure = false, Category = "Heart|GraphNode")
	FHeartGraphPinType GetInstancedPinType();


	/****************************/
	/**		NODE EDITING		*/
	/****************************/
public:
#if WITH_EDITOR
	void SetEdGraphNode(UEdGraphNode* GraphNode);

	bool SupportsDynamicPins_Editor() const;
#endif

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	void SetLocation(const FVector2D& NewLocation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode", DisplayName= "Supports Dynamic Pins", meta = (ScriptName = "SupportsDynamicPins"))
	bool SupportsDynamicPins_Runtime() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanUserAddInput() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanUserAddOutput() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanDelete() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanDuplicate() const;


	/****************************/
	/**		PIN EDITING			*/
	/****************************/

	template <typename THeartGraphPin>
	THeartGraphPin* CreatePin(EHeartPinDirection Direction, const FHeartGraphPinType Type)
	{
		static_assert(TIsDerivedFrom<THeartGraphPin, UHeartGraphPin>::IsDerived, "The pin class must derive from UHeartGraphPin");
		return Cast<THeartGraphPin>(CreatePin(THeartGraphPin::StaticClass(), Direction, Type));
	}

	template <typename THeartGraphPin>
	THeartGraphPin* CreatePin(const TSubclassOf<UHeartGraphPin> Class, FName Name, const EHeartPinDirection Direction, const FHeartGraphPinType Type)
	{
		static_assert(TIsDerivedFrom<THeartGraphPin, UHeartGraphPin>::IsDerived, "The pin class must derive from UHeartGraphPin");
		check(Class->IsChildOf<THeartGraphPin>());
		return Cast<THeartGraphPin>(CreatePinOfClass(Class, Name, Direction, Type));
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraphPin* CreatePin(FName Name, EHeartPinDirection Direction, const FHeartGraphPinType Type);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	UHeartGraphPin* CreatePinOfClass(TSubclassOf<UHeartGraphPin> Class, FName Name, EHeartPinDirection Direction, const FHeartGraphPinType Type);

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

#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	FOnNodeRefreshRequested OnReconstructionRequested;
#endif

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