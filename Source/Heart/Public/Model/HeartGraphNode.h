// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraph.h"
#include "HeartGraphPin.h"
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

UENUM(BlueprintType)
enum class EHeartNodeNameContext : uint8
{
	// Fallback when no special case needs to be considered
	Default,

	// Name shown on Graph Node instances. It is only valid to use NodeObject to determine the name under this context.
	NodeInstance,

	// Name shown in palettes or other lists
	Palette,
};

/**
 * Class data for UHeartGraph
 */
USTRUCT()
struct FHeartGraphNodeSparseClassData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Pins")
	TMap<FName, FHeartGraphPinType> DefaultInputs;

	UPROPERTY(EditDefaultsOnly, Category = "Pins")
	TMap<FName, FHeartGraphPinType> DefaultOutputs;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Editor", meta = (InlineEditConditionToggle))
	bool OverrideCanCreateInEditor = false;

	// Can this node be created by the editor even if it cannot be created otherwise.
	UPROPERTY(EditDefaultsOnly, Category = "Editor", meta = (EditCondition = "OverrideCanCreateInEditor"))
	bool CanCreateInEditor = false;

	// BP properties that trigger reconstruction of SGraphNodes
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	TArray<FName> PropertiesTriggeringNodeReconstruction;
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
	friend class UHeartEdGraphNode;

public:
	virtual UWorld* GetWorld() const override;

	virtual void PostLoad() override;

#if WITH_EDITOR
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif


	/*----------------------------
			REFLECTION
	----------------------------*/

	template<EHeartNodeNameContext Context>
	FText GetDefaultNodeTitle(const UClass* NodeClass) const
	{
		static_assert(Context != EHeartNodeNameContext::NodeInstance, TEXT("NodeInstance is not allowed in GetDefaultNodeTitle"));
		if (!IsValid(NodeClass)) return FText();
		return GetNodeTitle(NodeClass->GetDefaultObject(), Context);
	}

	FText GetDefaultNodeCategory(const UClass* NodeClass) const;
	FText GetDefaultNodeTooltip(const UClass* NodeClass) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetNodeTitle(const UObject* Node, EHeartNodeNameContext Context) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetNodeCategory(const UObject* Node) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetNodeToolTip(const UObject* Node) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool GetDynamicTitleColor(FLinearColor& LinearColor);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FText GetInstanceTitle() const;


	/*----------------------------
				GETTERS
	----------------------------*/

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

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraphPin* GetPinByName(const FName& Name);

	template <typename THeartGraphPin = UHeartGraphPin>
	TArray<THeartGraphPin*> GetPinsOfDirection(EHeartPinDirection Direction) const
	{
		static_assert(TIsDerivedFrom<THeartGraphPin, UHeartGraphPin>::IsDerived, "The pin class must derive from UHeartGraphPin");
		auto&& DirectedPins = GetPinsOfDirectionByClass(Direction, THeartGraphPin::StaticClass());
		return *reinterpret_cast<TArray<THeartGraphPin*>*>(&DirectedPins);
	}

	template <typename THeartGraphPin = UHeartGraphPin>
	TArray<THeartGraphPin*> GetInputPins() const { return GetPinsOfDirection<THeartGraphPin>(EHeartPinDirection::Input); }

	template <typename THeartGraphPin = UHeartGraphPin>
	TArray<THeartGraphPin*> GetOutputPins() const { return GetPinsOfDirection<THeartGraphPin>(EHeartPinDirection::Output); }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	TArray<UHeartGraphPin*> GetPinsOfDirection(EHeartPinDirection Direction) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	TArray<UHeartGraphPin*> GetPinsOfDirectionByClass(EHeartPinDirection Direction, TSubclassOf<UHeartGraphPin> Class) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	TArray<UHeartGraphPin*> GetInputPins(TSubclassOf<UHeartGraphPin> Class) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	TArray<UHeartGraphPin*> GetOutputPins(TSubclassOf<UHeartGraphPin> Class) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	uint8 GetUserInputNum() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	uint8 GetUserOutputNum() const;

	// Declare the pin typed used for instanced pins. Overriding this is required for User Input/Output to work.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure = false, Category = "Heart|GraphNode")
	FHeartGraphPinType GetInstancedPinType();


	/*----------------------------
			NODE EDITING
	----------------------------*/
public:
#if WITH_EDITOR
	void SetEdGraphNode(UEdGraphNode* GraphNode);

	virtual bool CanCreate_Editor() const;
#endif

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	void SetLocation(const FVector2D& NewLocation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanUserAddInput() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanUserAddOutput() const;

	// Can the user create instances of this node. Only necessary to override for use in graphs with the ability to spawn nodes.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanCreate() const;

	// Can the user delete this node. Only necessary to override for use in graphs with the ability to delete nodes.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanDelete() const;

	// Can the user duplicate this node. Only necessary to override for use in graphs with the ability to duplicate nodes.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	bool CanDuplicate() const;


	/*----------------------------
			PIN EDITING
	----------------------------*/

	template <typename THeartGraphPin>
	THeartGraphPin* CreatePin(EHeartPinDirection Direction, const FHeartGraphPinType& Type)
	{
		static_assert(TIsDerivedFrom<THeartGraphPin, UHeartGraphPin>::IsDerived, "The pin class must derive from UHeartGraphPin");
		return Cast<THeartGraphPin>(CreatePin(THeartGraphPin::StaticClass(), Direction, Type));
	}

	template <typename THeartGraphPin>
	THeartGraphPin* CreatePin(const TSubclassOf<UHeartGraphPin> Class, const FName Name, const EHeartPinDirection Direction, const FHeartGraphPinType& Type)
	{
		static_assert(TIsDerivedFrom<THeartGraphPin, UHeartGraphPin>::IsDerived, "The pin class must derive from UHeartGraphPin");
		check(Class->IsChildOf<THeartGraphPin>());
		return Cast<THeartGraphPin>(CreatePinOfClass(Class, Name, Direction, Type));
	}

	// Get all pins that match the predicate.
	template <typename Predicate>
	TArray<FHeartPinGuid> FindPinsByPredicate(EHeartPinDirection Direction, Predicate Pred) const;

	// Count all pins that match the predicate. Returns num of pins counted.
	template <typename Predicate>
	int32 CountPinsByPredicate(EHeartPinDirection Direction, Predicate Pred) const;

	// Remove all pins that match the predicate. Returns num of pins removed.
	template <typename Predicate>
	int32 RemovePinsByPredicate(EHeartPinDirection Direction, Predicate Pred);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraphPin* CreatePin(FName Name, EHeartPinDirection Direction, const FHeartGraphPinType Type);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	UHeartGraphPin* CreatePinOfClass(TSubclassOf<UHeartGraphPin> Class, FName Name, EHeartPinDirection Direction, const FHeartGraphPinType Type);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	void AddPin(UHeartGraphPin* Pin);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	bool RemovePin(UHeartGraphPin* Pin);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	bool RemovePinByGuid(FHeartPinGuid Pin);

	// Add a numbered instance pin
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraphPin* AddInstancePin(EHeartPinDirection Direction);

	// Remove the last numbered instance pin
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	void RemoveInstancePin(EHeartPinDirection Direction);

	virtual void NotifyPinConnectionsChanged(UHeartGraphPin* Pin);

protected:
	// Called by the owning graph when we are created.
	virtual void OnCreate();

	// Called by the owning graph when we are created.
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|GraphNode", DisplayName = "On Create")
	void BP_OnCreate();

	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|GraphNode", DisplayName = "On Connections Changed")
	void BP_OnConnectionsChanged(UHeartGraphPin* Pin);

public:
	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FOnPinConnectionsChanged OnPinConnectionsChanged;

	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FOnGraphNodePinChanged OnNodePinsChanged;

	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
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

	UPROPERTY(BlueprintReadOnly)
	TMap<FHeartPinGuid, TObjectPtr<UHeartGraphPin>> Pins;

private:
#if WITH_EDITORONLY_DATA
	// Always castable to UHeartEdGraphNode
	UPROPERTY()
	TObjectPtr<UEdGraphNode> HeartEdGraphNode;
#endif
};


/*----------------------------
		TEMPLATE IMPL.
----------------------------*/

template <typename Predicate>
TArray<FHeartPinGuid> UHeartGraphNode::FindPinsByPredicate(const EHeartPinDirection Direction, Predicate Pred) const
{
	TArray<FHeartPinGuid> MatchedPins;

	for (auto&& PinPair : Pins)
	{
		if (EnumHasAnyFlags(Direction, PinPair.Value->GetDirection()))
		{
			if (Pred(PinPair.Value))
			{
				MatchedPins.Add(PinPair.Key);
			}
		}
	}

	return MatchedPins;
}

template <typename Predicate>
int32 UHeartGraphNode::CountPinsByPredicate(const EHeartPinDirection Direction, Predicate Pred) const
{
	int32 PinCount = 0;

	for (auto&& PinPair : Pins)
	{
		if (EnumHasAnyFlags(Direction, PinPair.Value->GetDirection()))
		{
			if (Pred(PinPair.Value))
			{
				PinCount++;
			}
		}
	}

	return PinCount;
}

template <typename Predicate>
int32 UHeartGraphNode::RemovePinsByPredicate(const EHeartPinDirection Direction, Predicate Pred)
{
	TArray<FHeartPinGuid> PinsToRemove = FindPinsByPredicate(Direction, Pred);

	for (auto&& ToRemove : PinsToRemove)
	{
		Pins.Remove(ToRemove);
	}

	return PinsToRemove.Num();
}
