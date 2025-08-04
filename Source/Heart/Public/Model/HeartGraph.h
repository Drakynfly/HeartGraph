// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphInterface.h"
#include "HeartGraphNodeComponent.h"
#include "HeartGuids.h"
#include "HeartGraphTypes.h"
#include "HeartGraphPinReference.h"
#include "HeartNodeQuery.h"
#include "Templates/SubclassOf.h"
#include "UObject/Object.h"
#include "HeartGraph.generated.h"

namespace Heart::API
{
	class FNodeEdit;
	class FPinEdit;
}

class UHeartGraph;
class UHeartGraphExtension;
class UHeartGraphNode;
class UHeartGraphSchema;

DECLARE_LOG_CATEGORY_EXTERN(LogHeartGraph, Log, All)

namespace Heart::Events
{
	using FNodeAddOrRemove = TMulticastDelegate<void(UHeartGraphNode*)>;
	using FNodeMoveEventHandler = TMulticastDelegate<void(const FHeartNodeMoveEvent&)>;
	using FConnectionEventHandler = TMulticastDelegate<void(const FHeartGraphConnectionEvent&)>;

	using FGraphExtensionAddOrRemove = TMulticastDelegate<void(UHeartGraphExtension*)>;
	using FNodeComponentAddOrRemove = TMulticastDelegate<void(FHeartNodeGuid, UHeartGraphNodeComponent*)>;
}


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
	friend Heart::API::FNodeEdit;
	friend Heart::API::FPinEdit;

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
	FORCEINLINE virtual UHeartGraph* GetHeartGraph() const override final { return const_cast<ThisClass*>(this); }
	/* IHeartGraphInterface */

public:
	// @todo make these protected, gated behind an API (and rename to Handle...)
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
	class UEdGraph* GetEdGraph() const { return HeartEdGraph; }
	static FName GetSchemaClassPropertyName() { return GET_MEMBER_NAME_CHECKED(ThisClass, SchemaClass); }
#endif

	Heart::Events::FNodeAddOrRemove::RegistrationType& GetOnNodeAdded() { return OnNodeAdded; }
	Heart::Events::FNodeAddOrRemove::RegistrationType& GetOnNodeRemoved() { return OnNodeRemoved; }
	Heart::Events::FNodeMoveEventHandler::RegistrationType& GetOnNodeMoved() { return OnNodeMoved; }
	Heart::Events::FConnectionEventHandler::RegistrationType& GetOnNodeConnectionsChanged() { return OnNodeConnectionsChanged; }

	Heart::Events::FGraphExtensionAddOrRemove::RegistrationType& GetOnExtensionAdded() { return OnExtensionAdded; }
	Heart::Events::FGraphExtensionAddOrRemove::RegistrationType& GetOnExtensionRemoved() { return OnExtensionRemoved; }

	Heart::Events::FNodeComponentAddOrRemove::RegistrationType& GetOnNodeComponentAdded() { return OnComponentAdded; }
	Heart::Events::FNodeComponentAddOrRemove::RegistrationType& GetOnNodeComponentRemoved() { return OnComponentRemoved; }

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

	Heart::Query::FGraphNodeQuery QueryNodes() const;

protected:
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DisplayName = "Get Nodes"))
	const TMap<FHeartNodeGuid, UHeartGraphNode*>& BP_GetNodes() const { return ObjectPtrDecay(Nodes); }


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

	/** Adds a new extension of the requested class. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", Meta = (DeterminesOutputType = "Class"))
	UHeartGraphExtension* AddExtension(UPARAM(meta = (AllowAbstract = "false")) TSubclassOf<UHeartGraphExtension> Class);

	/** Adds an extension to the Extensions list. */
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


	/** Finds the first component of the template type. */
	template <
		typename TComponentClass
		UE_REQUIRES(TIsDerivedFrom<TComponentClass, UHeartGraphNodeComponent>::Value)
	>
	TComponentClass* GetNodeComponent(const FHeartNodeGuid& Node) const
	{
		return CastChecked<TComponentClass>(GetNodeComponent(Node, TComponentClass::StaticClass()), ECastCheckedType::NullAllowed);
	}

	/** Finds the first component of the requested class. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", Meta = (DeterminesOutputType = "Class", DisplayName = "Get Node Component"))
	UHeartGraphNodeComponent* GetNodeComponent(const FHeartNodeGuid& Node, TSubclassOf<UHeartGraphNodeComponent> Class) const;

	/** Finds all components for a node. (Iterates over all components; may be slow if graph has many node components!) */
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	TArray<UHeartGraphNodeComponent*> GetNodeComponentsForNode(const FHeartNodeGuid& Node) const;

	/** Finds all components of the requested class. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", Meta = (DeterminesOutputType = "Class"))
	TArray<UHeartGraphNodeComponent*> GetNodeComponentsOfClass(TSubclassOf<UHeartGraphNodeComponent> Class) const;

	/** Adds a new component of the requested class. If there is already a component of this class, the existing one will be returned instead. */
	template <
		typename TComponentClass
		UE_REQUIRES(TIsDerivedFrom<TComponentClass, UHeartGraphNodeComponent>::Value)
	>
	TComponentClass* FindOrAddNodeComponent(const FHeartNodeGuid& Node)
	{
		return CastChecked<TComponentClass>(FindOrAddNodeComponent(Node, TComponentClass::StaticClass()), ECastCheckedType::NullAllowed);
	}

	/** Adds a new component of the requested class. If there is already a component of this class, the existing one will be returned instead. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", Meta = (DeterminesOutputType = "Class"))
	UHeartGraphNodeComponent* FindOrAddNodeComponent(const FHeartNodeGuid& Node, UPARAM(meta = (AllowAbstract = "false")) TSubclassOf<UHeartGraphNodeComponent> Class);

	/** Remove a node component of a class. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	bool RemoveNodeComponent(const FHeartNodeGuid& Node, TSubclassOf<UHeartGraphNodeComponent> Class);

	/** Remove the component of the template class. */
	template <
		typename TComponentClass
		UE_REQUIRES(TIsDerivedFrom<TComponentClass, UHeartGraphNodeComponent>::Value)
	>
	bool RemoveNodeComponent(const FHeartNodeGuid& Node)
	{
		return RemoveNodeComponent(Node, TComponentClass::StaticClass());
	}

	void RemoveComponentsForNode(const FHeartNodeGuid& Node);
	void RemoveComponentsForNodes(TConstArrayView<FHeartNodeGuid> InNodes);


	/*----------------------------
			PIN EDITING
	----------------------------*/

	Heart::API::FPinEdit EditConnections();

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

	UPROPERTY(VisibleAnywhere, Category = "Graph")
	FHeartGraphGuid Guid;

	UPROPERTY(EditAnywhere, Category = "Graph", DisplayName = "Schema Override")
	TSubclassOf<UHeartGraphSchema> SchemaClass;

	UPROPERTY(Instanced, VisibleAnywhere, Category = "Graph")
	TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNode>> Nodes;

	// All extensions, including those added by the schema
	UPROPERTY(Instanced, VisibleAnywhere, Category = "Components")
	TMap<FHeartExtensionGuid, TObjectPtr<UHeartGraphExtension>> Extensions;

	// Extensions instanced for this asset
	UPROPERTY(Instanced, EditAnywhere, Category = "Components")
	TArray<TObjectPtr<UHeartGraphExtension>> InstancedExtensions;

	// All graph node components stored by class, and then by node
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TMap<TSubclassOf<UHeartGraphNodeComponent>, FHeartGraphNodeComponentMap> NodeComponents;

	Heart::Events::FNodeAddOrRemove OnNodeAdded;
	Heart::Events::FNodeAddOrRemove OnNodeRemoved;
	Heart::Events::FNodeMoveEventHandler OnNodeMoved;
	Heart::Events::FConnectionEventHandler OnNodeConnectionsChanged;

	Heart::Events::FGraphExtensionAddOrRemove OnExtensionAdded;
	Heart::Events::FGraphExtensionAddOrRemove OnExtensionRemoved;

	Heart::Events::FNodeComponentAddOrRemove OnComponentAdded;
	Heart::Events::FNodeComponentAddOrRemove OnComponentRemoved;


	/*----------------------------
			DEPRECATED API
	----------------------------*/

public:
	UE_DEPRECATED(5.5, "Please use NotifyNodeLocationChanged / NotifyNodeLocationChanged (Guid version) instead")
	virtual void NotifyNodeLocationsChanged(const TSet<UHeartGraphNode*>& AffectedNodes, bool InProgress);

protected:
	UE_DEPRECATED(5.5, "Please use HandleGraphConnectionEvent instead")
	virtual void NotifyNodeConnectionsChanged(const FHeartGraphConnectionEvent& Event);
};