// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGraphInterface.h"
#include "HeartGuids.h"
#include "HeartGraphTypes.h"
#include "HeartGraphPinReference.h"
#include "HeartGraph.generated.h"

class UHeartGraphSchema;
class UHeartGraphNode;

DECLARE_LOG_CATEGORY_EXTERN(LogHeartGraph, Log, All)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHeartGraphEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeartGraphNodeEvent, UHeartGraphNode*, Node);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeartGraphNodeConnectionEvent, const FHeartGraphConnectionEvent&, Event);

// @todo this struct only exists because of a bug in 5.2 preventing WITH_EDITORONLY_DATA from working in sparse
// If/when Epic fixes that, these properties should be moved back into the sparse class struct below
USTRUCT()
struct FHeartGraphEditorDataTemp
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Display")
	FText GraphTypeName;

	// Can the editor create instances of this graph as an asset.
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	bool CanCreateAssetFromFactory = false;

	// Should this class be shown in the "Common" section when creating a new Heart Graph
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	bool DisplayClassAsCommonInFactory = false;
#endif
};

/**
 * Class data for UHeartGraph
 */
USTRUCT()
struct FHeartGraphSparseClassData
{
	GENERATED_BODY()

/*
	UPROPERTY(EditDefaultsOnly, Category = "Display")
	FText GraphTypeName;

	// Can the editor create instances of this graph as an asset.
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	bool CanCreateAssetFromFactory = false;
*/
};

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable, SparseClassDataTypes = "HeartGraphSparseClassData")
class HEART_API UHeartGraph : public UObject, public IHeartGraphInterface
{
	GENERATED_BODY()

	friend class UHeartEdGraph;

public:
	virtual UWorld* GetWorld() const override;

	virtual void PostLoad() override;
	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;

	// Called after a pin connection change has been made.
	void NotifyNodeConnectionsChanged(const TArray<UHeartGraphNode*>& AffectedNodes, const TArray<FHeartPinGuid>& AffectedPins);

	// Called after a pin connection change has been made.
	virtual void NotifyNodeConnectionsChanged(const FHeartGraphConnectionEvent& Event);

#if WITH_EDITOR
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
#endif

	//* IHeartGraphInterface */
	virtual UHeartGraph* GetHeartGraph() const override final;
	//* IHeartGraphInterface */

	void ForEachNode(const TFunctionRef<bool(UHeartGraphNode*)>& Iter) const;


	/*-----------------------
			GETTERS
	------------------------*/
public:

#if WITH_EDITOR
	UEdGraph* GetEdGraph() const { return HeartEdGraph; }
#endif

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	FHeartGraphGuid GetGuid() const { return Guid; }

	template <typename THeartGraphNode>
	THeartGraphNode* GetNode(const FHeartNodeGuid& NodeGuid) const
	{
		static_assert(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::IsDerived, "THeartGraphNode must derive from UHeartGraphNode");
		return Cast<THeartGraphNode>(GetNode(NodeGuid));
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	UHeartGraphNode* GetNode(const FHeartNodeGuid& NodeGuid) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|Graph")
	void GetNodeArray(TArray<UHeartGraphNode*>& OutNodes) const
	{
		// *le sign* epic templates mess this up . . .
		TArray<TObjectPtr<UHeartGraphNode>> NodeArray;
		Nodes.GenerateValueArray(NodeArray);
		OutNodes = NodeArray;
	}


	/*----------------------------
			CLASS BEHAVIOR
	----------------------------*/
public:
	/** Override to specify the behavior class for this graph class */
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|Graph")
	TSubclassOf<UHeartGraphSchema> GetSchemaClass() const;

	template <typename THeartGraphSchema, typename THeartGraph>
	static const THeartGraphSchema* GetSchemaStatic()
	{
		static_assert(TIsDerivedFrom<THeartGraphSchema, UHeartGraphSchema>::IsDerived, "THeartGraphSchema must derive from UHeartGraphSchema");
		static_assert(TIsDerivedFrom<THeartGraph, UHeartGraph>::IsDerived, "THeartGraph must derive from UHeartGraph");
		const THeartGraph* DefaultHeartGraph = GetDefault<THeartGraph>();
		return GetDefault<THeartGraphSchema>(DefaultHeartGraph->GetSchemaClass());
	}

	template <typename THeartGraphSchema>
	static const THeartGraphSchema* GetSchemaStatic(const TSubclassOf<UHeartGraph> GraphClass)
	{
		static_assert(TIsDerivedFrom<THeartGraphSchema, UHeartGraphSchema>::IsDerived, "THeartGraphSchema must derive from UHeartGraphSchema");
		const UHeartGraph* DefaultHeartGraph = GetDefault<UHeartGraph>(GraphClass);
		return GetDefault<THeartGraphSchema>(DefaultHeartGraph->GetSchemaClass());
	}

	template <typename THeartGraphSchema>
	const THeartGraphSchema* GetSchemaTyped() const
	{
		static_assert(TIsDerivedFrom<THeartGraphSchema, UHeartGraphSchema>::IsDerived, "THeartGraphSchema must derive from UHeartGraphSchema");
		return Cast<THeartGraphSchema>(GetSchema());
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	const UHeartGraphSchema* GetSchema() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DeterminesOutputType = Class), DisplayName = "Get Schema Typed")
	const UHeartGraphSchema* GetSchemaTyped_K2(TSubclassOf<UHeartGraphSchema> Class) const;


	/*----------------------------
			NODE EDITING
	----------------------------*/
private:
	UHeartGraphNode* CreateNodeForNodeObject(UObject* NodeObject, const FVector2D& Location);
	UHeartGraphNode* CreateNodeForNodeClass(const UClass* NodeClass, const FVector2D& Location);

public:
	// Create from template graph class and node object
	template <typename THeartGraphNode>
	THeartGraphNode* CreateNodeFromObject(UObject* NodeObject, const FVector2D& Location)
	{
		static_assert(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::IsDerived, "THeartGraphNode must derive from UHeartGraphNode");
		checkf(!NodeObject->IsA<UHeartGraphNode>(), TEXT("If this trips, you've passed in a 'GRAPH' node object instead of an 'OBJECT' node class"));
		return Cast<THeartGraphNode>(CreateNodeForNodeObject(NodeObject, Location));
	}

	// Create from template node class and attempt to cast the return to the template graph class
	// Note that this does not enforce the created GraphNode to be of the templated graph node class.
	template <typename THeartGraphNode, typename THeartNode>
	THeartGraphNode* CreateNodeFromClass(const FVector2D& Location)
	{
		static_assert(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::IsDerived, "THeartGraphNode must derive from UHeartGraphNode");
		static_assert(!TIsDerivedFrom<THeartNode, UHeartGraphNode>::IsDerived, "THeartNode must not derive from UHeartGraphNode");
		return Cast<THeartGraphNode>(CreateNodeForNodeClass(THeartNode::StaticClass(), Location));
	}

	// Create from node class and attempt to cast the return to the template graph class
	// Note that this does not enforce the created GraphNode to be of the templated graph node class.
	template <typename THeartGraphNode>
	THeartGraphNode* CreateNodeFromClass(const TSubclassOf<UObject> NodeClass, const FVector2D& Location)
	{
		static_assert(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::IsDerived, "THeartGraphNode must derive from UHeartGraphNode");
		checkf(!NodeClass->IsChildOf<THeartGraphNode>(), TEXT("If this trips, you've passed in a 'GRAPH' node class instead of an 'OBJECT' node class"));
		return Cast<THeartGraphNode>(CreateNodeForNodeClass(NodeClass, Location));
	}

	/**
	 * Create a new node, spawning a new NodeObject from the NodeClass provided.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraphNode* CreateNodeFromClass(const UClass* NodeClass, const FVector2D& Location);

	/**
	 * Create a new node, using the provided NodeObject
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraphNode* CreateNodeFromObject(UObject* NodeObject, const FVector2D& Location);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	void AddNode(UHeartGraphNode* Node);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	bool RemoveNode(const FHeartNodeGuid& NodeGuid);

	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|Graph")
	void BP_OnNodeConnectionsChanged(const FHeartGraphConnectionEvent& ConnectionEvent);


	/*----------------------------
			PIN EDITING
	----------------------------*/

	bool ConnectPins(FHeartGraphPinReference PinA, FHeartGraphPinReference PinB);

	bool DisconnectPins(FHeartGraphPinReference PinA, FHeartGraphPinReference PinB);

	void DisconnectAllPins(FHeartGraphPinReference Pin);

public:
	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FHeartGraphNodeEvent OnNodeAdded;

	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FHeartGraphNodeEvent OnNodeRemoved;

	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FHeartGraphNodeConnectionEvent OnNodeConnectionsChanged;

#if WITH_EDITORONLY_DATA
private:
	// Always castable to UHeartEdGraph. Only valid for HeartGraphs created by the editor.
	UPROPERTY()
	TObjectPtr<UEdGraph> HeartEdGraph;

	// Delegate that is broadcast whenever a node is added to the graph while in the editor, from a source other than
	// the HeartEdGraph, such as a BP schema, or action. This allows the EdGraph to assign an EdGraphNode anyway, which
	// is needed to visualize the node in the editor.
	DECLARE_DELEGATE_OneParam(FNodeCreatedInEditorExternally, UHeartGraphNode* /* Node */)
	FNodeCreatedInEditorExternally OnNodeCreatedInEditorExternally;

	// @todo temp while sparse struct is broken, see above comment on this
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	FHeartGraphEditorDataTemp EditorData;
public:
	auto GetGraphTypeName() const { return EditorData.GraphTypeName; }
	auto GetCanCreateAssetFromFactory() const { return EditorData.CanCreateAssetFromFactory; }
	auto GetDisplayClassAsCommonInFactory() const { return EditorData.DisplayClassAsCommonInFactory; }
private:
#endif

private:
	UPROPERTY()
	FHeartGraphGuid Guid;

	UPROPERTY()
	TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNode>> Nodes;
};