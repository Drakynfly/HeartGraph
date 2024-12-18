﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGraphInterface.h"
#include "HeartGuids.h"
#include "HeartGraphTypes.h"
#include "HeartGraphPinReference.h"
#include "HeartPinConnectionEdit.h"
#include "HeartGraph.generated.h"

namespace Heart::API
{
	class FNodeEdit;
}

class UHeartGraph;
class UHeartGraphExtension;
class UHeartGraphNode;
class UHeartGraphSchema;

DECLARE_LOG_CATEGORY_EXTERN(LogHeartGraph, Log, All)

using FHeartGraphNodeEvent = TMulticastDelegate<void(UHeartGraphNode*)>;
using FHeartGraphNodeMovedEvent = TMulticastDelegate<void(const FHeartNodeMoveEvent&)>;
using FHeartGraphNodeConnectionEvent = TMulticastDelegate<void(const FHeartGraphConnectionEvent&)>;

using FHeartGraphExtensionEvent = TMulticastDelegate<void(UHeartGraphExtension*)>;

/**
 * Class data for UHeartGraph
 */
USTRUCT()
struct FHeartGraphSparseClassData
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	/**
	 * The displayed name of graphs in the Unreal Editor graph window corner text. By default, will read "Heart" unless
	 * changed by a subclass, either in BP via the details panel, or in C++ by settings
	 * GetHeartGraphSparseClassData()->GraphTypeName in the constructor.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Display")
	FText GraphTypeName;

	// Can the editor create instances of this graph as an asset?
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	bool CanCreateAssetFromFactory = false;

	// Should this class be shown in the "Common" section when creating a new Heart Graph?
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	bool DisplayClassAsCommonInFactory = false;
#endif
};

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable, SparseClassDataTypes = "HeartGraphSparseClassData")
class HEART_API UHeartGraph : public UObject, public IHeartGraphInterface
{
	GENERATED_BODY()

	friend class UHeartEdGraph;
	friend class UHeartGraphSchema;
	friend class Heart::API::FNodeEdit;
	friend class Heart::Connections::FEdit;

public:
	UHeartGraph();

#if WITH_EDITOR
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
#endif

	virtual UWorld* GetWorld() const override;

	/* UObject */
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
	/* UObject */

private:
	/* IHeartGraphInterface */
	virtual UHeartGraph* GetHeartGraph() const override final;
	/* IHeartGraphInterface */

public:
	// @tode make these protected, gated behind an API (and rename to Handle...)
	void NotifyNodeLocationChanged(const FHeartNodeGuid& AffectedNode, bool InProgress);
	void NotifyNodeLocationsChanged(const TSet<FHeartNodeGuid>& AffectedNodes, bool InProgress);

	// Return true in Iter to continue iterating
	void ForEachNode(const TFunctionRef<bool(UHeartGraphNode*)>& Iter) const;

	// Return true in Iter to continue iterating
	void ForEachExtension(const TFunctionRef<bool(UHeartGraphExtension*)>& Iter) const;

protected:
	// Called after nodes have been added.
	virtual void HandleNodeAddEvent(const FHeartNodeAddEvent& Event);

	// Called before nodes have been removed (nodes are still valid at the time of call).
	virtual void HandleNodeRemoveEvent(const FHeartNodeRemoveEvent& Event);

	// Called after node locations have changed.
	virtual void HandleNodeMoveEvent(const FHeartNodeMoveEvent& Event);

	// Called after a pin connection change has been made. Called by Heart::Connections::~FEdit
	virtual void HandleGraphConnectionEvent(const FHeartGraphConnectionEvent& Event);


	/*-----------------------
			GETTERS
	------------------------*/
public:
#if WITH_EDITOR
	UEdGraph* GetEdGraph() const { return HeartEdGraph; }
#endif

	FHeartGraphNodeEvent::RegistrationType& GetOnNodeAdded() { return OnNodeAdded; }
	FHeartGraphNodeEvent::RegistrationType& GetOnNodeRemoved() { return OnNodeRemoved; }
	FHeartGraphNodeMovedEvent::RegistrationType& GetOnNodeMoved() { return OnNodeMoved; }
	FHeartGraphNodeConnectionEvent::RegistrationType& GetOnNodeConnectionsChanged() { return OnNodeConnectionsChanged; }

	FHeartGraphExtensionEvent::RegistrationType& GetOnExtensionAdded() { return OnExtensionAdded; }
	FHeartGraphExtensionEvent::RegistrationType& GetOnExtensionRemoved() { return OnExtensionRemoved; }

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	FHeartGraphGuid GetGuid() const { return Guid; }

	template <
		typename THeartGraphNode
		UE_REQUIRES(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::Value)
	>
	THeartGraphNode* GetNode(const FHeartNodeGuid& NodeGuid) const
	{
		return Cast<THeartGraphNode>(GetNode(NodeGuid));
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	UHeartGraphNode* GetNode(const FHeartNodeGuid& NodeGuid) const;

	const auto& GetNodes() const { return Nodes; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|Graph")
	void GetNodeGuids(TArray<FHeartNodeGuid>& OutGuids) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|Graph")
	void GetNodeArray(TArray<UHeartGraphNode*>& OutNodes) const;

protected:
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DisplayName = "GetNodes"))
	const TMap<FHeartNodeGuid, UHeartGraphNode*>& BP_GetNodes() const { return reinterpret_cast<const TMap<FHeartNodeGuid, UHeartGraphNode*>&>(Nodes); }


	/*----------------------------
			CLASS BEHAVIOR
	----------------------------*/
protected:
	/** Override to specify the behavior class for this graph class */
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|Graph")
	TSubclassOf<UHeartGraphSchema> GetSchemaClass() const;

public:
	template <
		typename THeartGraphSchema
		UE_REQUIRES(TIsDerivedFrom<THeartGraphSchema, UHeartGraphSchema>::Value)
	>
	const THeartGraphSchema* GetSchemaTyped() const
	{
		return Cast<THeartGraphSchema>(GetSchema());
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	const UHeartGraphSchema* GetSchema() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DeterminesOutputType = Class), DisplayName = "Get Schema Typed")
	const UHeartGraphSchema* GetSchemaTyped_K2(TSubclassOf<UHeartGraphSchema> Class) const;

	const auto& GetExtensions() const { return Extensions; }

	/** Finds the first extension of the template type. */
	template <
		typename TExtensionClass
		UE_REQUIRES(TIsDerivedFrom<TExtensionClass, UHeartGraphExtension>::Value)
	>
	TExtensionClass* GetExtension() const
	{
		return CastChecked<TExtensionClass>(GetExtension(TExtensionClass::StaticClass()), ECastCheckedType::NullAllowed);
	}

	/** Finds the first extension of the requested class. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", Meta = (DeterminesOutputType = "Class", DisplayName = "Get Extension (by guid)"))
	UHeartGraphExtension* GetExtensionByGuid(FHeartExtensionGuid ExtensionGuid, TSubclassOf<UHeartGraphExtension> Class = nullptr) const;

	/** Finds the first extension of the requested class. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", Meta = (DeterminesOutputType = "Class", DisplayName = "Get Extension (by class)"))
	UHeartGraphExtension* GetExtension(TSubclassOf<UHeartGraphExtension> Class) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	TArray<UHeartGraphExtension*> GetAllExtensions() const;

	/** Finds all extensions of the requested class. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", Meta = (DeterminesOutputType = "Class"))
	TArray<UHeartGraphExtension*> GetExtensionsOfClass(TSubclassOf<UHeartGraphExtension> Class) const;

	/** Add the extension of the template class. */
	template <
		typename TExtensionClass
		UE_REQUIRES(TIsDerivedFrom<TExtensionClass, UHeartGraphExtension>::Value)
	>
	TExtensionClass* AddExtension()
	{
		return CastChecked<TExtensionClass>(AddExtension(TExtensionClass::StaticClass()), ECastCheckedType::NullAllowed);
	}

	/** Add the extension of the requested class. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", Meta = (DeterminesOutputType = "Class"))
	UHeartGraphExtension* AddExtension(UPARAM(meta = (AllowAbstract = "false")) TSubclassOf<UHeartGraphExtension> Class);

	/** Adds an extension to the Extensions list. Fails if an extension of this class already exists. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	bool AddExtensionInstance(UHeartGraphExtension* Extension);

	/** Remove the extension with this guid. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	bool RemoveExtension(FHeartExtensionGuid ExtensionGuid);

	/** Remove all extensions of a class. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	void RemoveExtensionsByClass(TSubclassOf<UHeartGraphExtension> Class);

	/** Remove the extension of the template class. */
	template <
		typename TExtensionClass
		UE_REQUIRES(TIsDerivedFrom<TExtensionClass, UHeartGraphExtension>::Value)
	>
	void RemoveExtensionsByClass()
	{
		return RemoveExtensionsByClass(TExtensionClass::StaticClass());
	}


	/*----------------------------
			NODE EDITING
	----------------------------*/
public:
	// Create a HeartGraphNode that is the outer of its own instanced NodeObject, created from the NodeObjectClass.
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DisplayName = "Create Node (instanced)"))
	UHeartGraphNode* CreateNode_Instanced(TSubclassOf<UHeartGraphNode> GraphNodeClass, const UClass* NodeObjectClass, const FVector2D& Location);

	// Create a HeartGraphNode whose NodeObject is a reference to an external object.
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DisplayName = "Create Node (reference)"))
	UHeartGraphNode* CreateNode_Reference(TSubclassOf<UHeartGraphNode> GraphNodeClass, const UObject* NodeObject, const FVector2D& Location);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	void AddNode(UHeartGraphNode* Node);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	bool RemoveNode(const FHeartNodeGuid& NodeGuid);

	// @todo move to extension
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|Graph")
	void BP_OnNodeConnectionsChanged(const FHeartGraphConnectionEvent& ConnectionEvent);


	/*----------------------------
			PIN EDITING
	----------------------------*/

	Heart::Connections::FEdit EditConnections();

	bool ConnectPins(const FHeartGraphPinReference& PinA, const FHeartGraphPinReference& PinB);

	bool DisconnectPins(const FHeartGraphPinReference& PinA, const FHeartGraphPinReference& PinB);

	bool DisconnectAllPins(const FHeartGraphPinReference& Pin);


	/*----------------------------
			PRIVATE STATE
	----------------------------*/
private:
#if WITH_EDITORONLY_DATA
	// Always castable to UHeartEdGraph. Only valid for HeartGraphs created by the editor.
	UPROPERTY()
	TObjectPtr<UEdGraph> HeartEdGraph;
#endif

	UPROPERTY()
	FHeartGraphGuid Guid;

	UPROPERTY(Instanced)
	TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNode>> Nodes;

	UPROPERTY(Instanced, VisibleAnywhere)
	TMap<FHeartExtensionGuid, TObjectPtr<UHeartGraphExtension>> Extensions;

	FHeartGraphNodeEvent OnNodeAdded;
	FHeartGraphNodeEvent OnNodeRemoved;
	FHeartGraphNodeMovedEvent OnNodeMoved;
	FHeartGraphNodeConnectionEvent OnNodeConnectionsChanged;

	FHeartGraphExtensionEvent OnExtensionAdded;
	FHeartGraphExtensionEvent OnExtensionRemoved;


	/*----------------------------
			DEPRECATED API
	----------------------------*/

public:
	template <
		typename THeartGraphNode
		UE_REQUIRES(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::Value)
	>
	UE_DEPRECATED(5.3, "Use Heart::API::FNodeEdit instead")
	THeartGraphNode* CreateNodeFromObject(UObject* NodeObject, const FVector2D& Location)
	{
		checkf(!NodeObject->IsA<UHeartGraphNode>(), TEXT("If this trips, you've passed in a 'GRAPH' node object instead of an 'OBJECT' node class"));
		return Cast<THeartGraphNode>(CreateNode_Reference(THeartGraphNode::StaticClass(), NodeObject, Location));
	}

	template <
		typename THeartGraphNode,
		typename TNodeObject
		UE_REQUIRES(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::Value &&
					!TIsDerivedFrom<TNodeObject, UHeartGraphNode>::Value)
	>
	UE_DEPRECATED(5.3, "Use Heart::API::FNodeEdit instead")
	THeartGraphNode* CreateNodeFromClass(const FVector2D& Location)
	{
		return Cast<THeartGraphNode>(CreateNode_Instanced(THeartGraphNode::StaticClass(), TNodeObject::StaticClass(), Location));
	}

	template <
		typename THeartGraphNode
		UE_REQUIRES(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::Value)
	>
	UE_DEPRECATED(5.3, "Use Heart::API::FNodeEdit instead")
	THeartGraphNode* CreateNodeFromClass(const TSubclassOf<UObject> NodeClass, const FVector2D& Location)
	{
		checkf(!NodeClass->IsChildOf<THeartGraphNode>(), TEXT("If this trips, you've passed in a 'GRAPH' node class instead of an 'OBJECT' node class"));
		return Cast<THeartGraphNode>(CreateNode_Instanced(THeartGraphNode::StaticClass(), NodeClass, Location));
	}

	/**
	 * Create a new node, spawning a new NodeObject from the NodeClass provided.
	 */
	UE_DEPRECATED(5.3, "Use CreateNode_Instanced instead")
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraphNode* CreateNodeFromClass(const UClass* NodeClass, const FVector2D& Location);

	/**
	 * Create a new node, using the provided NodeObject
	 */
	UE_DEPRECATED(5.3, "Use CreateNode_Reference instead")
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraphNode* CreateNodeFromObject(UObject* NodeObject, const FVector2D& Location);

	UE_DEPRECATED(5.5, "Please use NotifyNodeLocationChangedNotifyNodeLocationChanged instead")
	virtual void NotifyNodeLocationsChanged(const TSet<UHeartGraphNode*>& AffectedNodes, bool InProgress);

protected:
	UE_DEPRECATED(5.5, "Please use HandleGraphConnectionEvent instead")
	virtual void NotifyNodeConnectionsChanged(const FHeartGraphConnectionEvent& Event);
};