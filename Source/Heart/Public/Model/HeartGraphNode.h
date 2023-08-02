// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

#include "HeartGraphNodeInterface.h"
#include "HeartGraphPinDesc.h"
#include "HeartGraphPinTag.h"
#include "HeartGraphPinReference.h"
#include "GraphRegistry/HeartNodeSource.h"
#include "Model/HeartGuids.h"
#include "Model/HeartPinDirection.h"

#include "HeartGraphNode.generated.h"

struct FHeartGraphNodeMessage;
class UHeartGraph;
class UHeartGraphCanvas;
class UHeartGraphNode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPinConnectionsChanged, const FHeartPinGuid&, Pin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGraphNodePinChanged, UHeartGraphNode*, Node);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGraphNodeLocationChanged, UHeartGraphNode*, Node, const FVector2D&, Location);

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

// @todo this struct only exists because of a bug in 5.2 preventing WITH_EDITORONLY_DATA from working in sparse
// If/when Epic fixes that, these properties should be moved back into the sparse class struct below
USTRUCT()
struct FHeartGraphNodeEditorDataTemp
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Editor", meta = (InlineEditConditionToggle))
	bool OverrideCanCreateInEditor = false;

	// Can this node be created by the editor even if it cannot be created otherwise.
	UPROPERTY(EditDefaultsOnly, Category = "Editor", meta = (EditCondition = "OverrideCanCreateInEditor"))
	bool CanCreateInEditor = false;

	// BP properties that trigger reconstruction of SGraphNodes
	// @todo long term solution is to replace this with custom metadata on the BP properties that adds TriggersReconstruct
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	TArray<FName> PropertiesTriggeringNodeReconstruction;

	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	FName EditorSlateStyle;
#endif
};

/**
 * Class data for UHeartGraphNode
 */
USTRUCT()
struct FHeartGraphNodeSparseClassData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Pins")
	TArray<FHeartGraphPinDesc> DefaultPins;

	UPROPERTY(EditDefaultsOnly, Category = "Pins")
	uint8 DefaultInstancedInputs = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Pins")
	uint8 DefaultInstancedOutputs = 0;

	/*
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Editor", meta = (InlineEditConditionToggle))
	bool OverrideCanCreateInEditor = false;

	// Can this node be created by the editor even if it cannot be created otherwise.
	UPROPERTY(EditDefaultsOnly, Category = "Editor", meta = (EditCondition = "OverrideCanCreateInEditor"))
	bool CanCreateInEditor = false;

	// BP properties that trigger reconstruction of SGraphNodes
	// @todo long term solution is to replace this with custom metadata on the BP properties that adds TriggersReconstruct
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	TArray<FName> PropertiesTriggeringNodeReconstruction;

	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	FName EditorSlateStyle;
#endif
	*/
};

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable, SparseClassDataTypes = "HeartGraphNodeSparseClassData")
class HEART_API UHeartGraphNode : public UObject, public IHeartGraphNodeInterface
{
	GENERATED_BODY()

	friend class UHeartGraph;
	friend class UHeartGraphSchema;
	friend class UHeartEdGraphNode;

public:
	UHeartGraphNode();

	virtual UWorld* GetWorld() const override;

	virtual void PostLoad() override;

#if WITH_EDITOR
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

	/** IHeartGraphNodeInterface */
	virtual UHeartGraphNode* GetHeartGraphNode() const override final;
	/** IHeartGraphNodeInterface */


	/*----------------------------
			REFLECTION
	----------------------------*/

	template<EHeartNodeNameContext Context>
	FText GetDefaultNodeTitle(FHeartNodeSource NodeSource) const;

	FText GetDefaultNodeCategory(FHeartNodeSource NodeSource) const;
	FText GetDefaultNodeTooltip(FHeartNodeSource NodeSource) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetNodeTitle(const UObject* Node, EHeartNodeNameContext Context) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetNodeCategory(const UObject* Node) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FText GetNodeToolTip(const UObject* Node) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	FLinearColor GetNodeTitleColor(const UObject* Node);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|GraphNode")
	void GetNodeMessages(TArray<FHeartGraphNodeMessage>& Messages) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FText GetInstanceTitle() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	uint8 GetInstancedInputNum() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	uint8 GetInstancedOutputNum() const;


	/*----------------------------
				GETTERS
	----------------------------*/

	template <typename TNodeClass>
	TNodeClass* GetNodeObject() const
	{
		return Cast<TNodeClass>(NodeObject);
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UObject* GetNodeObject() const { return NodeObject; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeprecatedFunction, DeterminesOutputType = Class))
	UObject* GetNodeObjectTyped(TSubclassOf<UObject> Class) const { return NodeObject; }

	template <typename THeartGraph>
	THeartGraph* GetOwningGraph() const
	{
		static_assert(TIsDerivedFrom<THeartGraph, UHeartGraph>::IsDerived, "The graph class must derive from UHeartGraph");
		return Cast<THeartGraph>(GetGraph());
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraph* GetGraph() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = "Class", DeprecatedFunction))
	UHeartGraph* GetGraphTyped(TSubclassOf<UHeartGraph> Class) const { return GetGraph(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FHeartNodeGuid GetGuid() const { return Guid; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FVector2D GetLocation() const { return Location; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FHeartGraphPinDesc GetPinDesc(const FHeartPinGuid& Pin) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FHeartGraphPinReference GetPinReference(const FHeartPinGuid& Pin) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (AutoCreateRefTerm = "Name"))
	FHeartPinGuid GetPinByName(const FName& Name) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	TArray<FHeartPinGuid> GetPinsOfDirection(EHeartPinDirection Direction) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	TArray<FHeartPinGuid> GetInputPins() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNode")
	TArray<FHeartPinGuid> GetOutputPins() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Heart|GraphNode")
	TArray<FHeartGraphPinDesc> GetDynamicPins() const;

	// Declare the pin typed used for instanced pins. Overriding this is required for User Input/Output to work.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure = false, Category = "Heart|GraphNode")
	void GetInstancedPinData(EHeartPinDirection Direction, FHeartGraphPinTag& Tag, TArray<UHeartGraphPinMetadata*>& Metadata) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	bool HasConnections(const FHeartPinGuid& Pin) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	TSet<UHeartGraphNode*> GetConnectedGraphNodes(EHeartPinDirection Direction = EHeartPinDirection::Bidirectional) const;


	/*----------------------------
			NODE EDITING
	----------------------------*/
public:
#if WITH_EDITOR
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

	FHeartGraphPinConnections& GetLinks(const FHeartPinGuid& Pin);
	FHeartGraphPinConnections GetLinks(const FHeartPinGuid& Pin) const;

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
	FHeartPinGuid AddPin(const FHeartGraphPinDesc& Desc);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	bool RemovePin(const FHeartPinGuid& Pin);

	// Add a numbered instance pin
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	FHeartPinGuid AddInstancePin(EHeartPinDirection Direction);

	// Remove the last numbered instance pin
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	void RemoveInstancePin(EHeartPinDirection Direction);

	virtual void NotifyPinConnectionsChanged(const FHeartPinGuid& Pin);

protected:
	// Called by the owning graph when we are created.
	virtual void OnCreate();

	void ReconstructPins();

	// Called by the owning graph when we are created.
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|GraphNode", DisplayName = "On Create")
	void BP_OnCreate();

	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|GraphNode", DisplayName = "On Connections Changed")
	void BP_OnConnectionsChanged(FHeartPinGuid Pin);

public:
	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FOnPinConnectionsChanged OnPinConnectionsChanged;

	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FOnGraphNodePinChanged OnNodePinsChanged;

	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FOnGraphNodeLocationChanged OnNodeLocationChanged;

#if WITH_EDITORONLY_DATA
protected:
	DECLARE_DELEGATE(FOnNodeRefreshRequested)
	FOnNodeRefreshRequested OnReconstructionRequested;

	// @todo temp while sparse struct is broken, see above comment on this
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	FHeartGraphNodeEditorDataTemp EditorData;
public:
	auto GetOverrideCanCreateInEditor() const { return EditorData.OverrideCanCreateInEditor; }
	auto GetCanCreateInEditor() const { return EditorData.CanCreateInEditor; }
	auto GetPropertiesTriggeringNodeReconstruction() const { return EditorData.PropertiesTriggeringNodeReconstruction; }
	auto GetEditorSlateStyle() const { return EditorData.EditorSlateStyle; }
#endif

protected:
	// The object that this graph node represents. Contains the data and functionality of a spawned instance.
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UObject> NodeObject;

	UPROPERTY()
	FHeartNodeGuid Guid;

	UPROPERTY()
	FVector2D Location;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TMap<FHeartPinGuid, FHeartGraphPinDesc> PinDescriptions;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TMap<FHeartPinGuid, FHeartGraphPinConnections> PinConnections;

	UPROPERTY(BlueprintReadOnly)
	uint8 InstancedInputs = 0;

	UPROPERTY(BlueprintReadOnly)
	uint8 InstancedOutputs = 0;
};


/*----------------------------
		TEMPLATE IMPL.
----------------------------*/

template <EHeartNodeNameContext Context>
FText UHeartGraphNode::GetDefaultNodeTitle(const FHeartNodeSource NodeSource) const
{
	static_assert(Context != EHeartNodeNameContext::NodeInstance, TEXT("NodeInstance is not allowed for GetDefaultNodeTitle"));
	if (!NodeSource.IsValid()) return FText();
	return GetNodeTitle(NodeSource.GetDefaultObject(), Context);
}

template <typename Predicate>
TArray<FHeartPinGuid> UHeartGraphNode::FindPinsByPredicate(const EHeartPinDirection Direction, Predicate Pred) const
{
	TArray<FHeartPinGuid> MatchedPins;

	for (const TTuple<FHeartPinGuid, FHeartGraphPinDesc>& PinPair : PinDescriptions)
	{
		if (EnumHasAnyFlags(Direction, PinPair.Value.Direction))
		{
			if (Pred(PinPair))
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

	for (auto&& PinPair : PinDescriptions)
	{
		if (EnumHasAnyFlags(Direction, PinPair.Value.Direction))
		{
			if (Pred(PinPair))
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
		RemovePin(ToRemove);
	}

	return PinsToRemove.Num();
}