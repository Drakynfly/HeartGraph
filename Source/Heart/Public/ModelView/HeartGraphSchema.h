// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGraphSchema.generated.h"

struct FHeartGraphPinReference;
class UHeartGraph;
class UHeartGraphNodeRegistry;
class UHeartGraphExtension;
class UHeartGraphAction;

/**
 * This is the type of response the graph editor should take when making a connection
 * WARNING: Must have same order as ECanCreateConnectionResponse!!!
 */
UENUM(BlueprintType)
enum class EHeartCanConnectPinsResponse : uint8
{
	/** Make the connection; there are no issues (message string is displayed if not empty). */
	Allow,

	/** Cannot make this connection; display the message string as an error. */
	Disallow,

	/** Break all existing connections on A and make the new connection (it's exclusive); display the message string as a warning/notice. */
	AllowBreakA,

	/** Break all existing connections on B and make the new connection (it's exclusive); display the message string as a warning/notice. */
	AllowBreakB,

	/** Break all existing connections on A and B, and make the new connection (it's exclusive); display the message string as a warning/notice. */
	AllowBreakAB,

	//** Make the connection via an intermediate cast node, or some other conversion node. */
	//CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE,

	/** Make the connection by promoting a lower type to a higher type. Ex: Connecting a Float -> Double, float should become a double */
	//CONNECT_RESPONSE_MAKE_WITH_PROMOTION,

	//CONNECT_RESPONSE_MAX,
};

USTRUCT(BlueprintType)
struct FHeartConnectPinsResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ConnectPinsResponse")
	EHeartCanConnectPinsResponse Response = EHeartCanConnectPinsResponse::Allow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ConnectPinsResponse")
	FText Message;

	static FHeartConnectPinsResponse Disallow(const FText& Message)
	{
		return FHeartConnectPinsResponse{EHeartCanConnectPinsResponse::Disallow, Message };
	}

	static FHeartConnectPinsResponse Allow()
	{
		return FHeartConnectPinsResponse{EHeartCanConnectPinsResponse::Allow, FText::GetEmpty() };
	}

	static FHeartConnectPinsResponse BreakA()
	{
		return FHeartConnectPinsResponse{EHeartCanConnectPinsResponse::AllowBreakA, FText::GetEmpty() };
	}

	static FHeartConnectPinsResponse BreakB()
	{
		return FHeartConnectPinsResponse{EHeartCanConnectPinsResponse::AllowBreakB, FText::GetEmpty() };
	}

	static FHeartConnectPinsResponse BreakAB()
	{
		return FHeartConnectPinsResponse{EHeartCanConnectPinsResponse::AllowBreakAB, FText::GetEmpty() };
	}
};

/**
 * Base class for Heart Schemas, const classes that are interacted with via only their CDO to describe the behavior
 * of a Heart Graph instance.
 */
UCLASS(Abstract, Const, BlueprintType, Blueprintable)
class HEART_API UHeartGraphSchema : public UObject // Based on UEdGraphSchema
{
	GENERATED_BODY()

	friend UHeartGraph;

public:
	UHeartGraphSchema();

	static const UHeartGraphSchema* Get(const TSubclassOf<UHeartGraph> GraphClass);

	template <typename THeartGraph>
	static const UHeartGraphSchema* Get()
	{
		static_assert(TIsDerivedFrom<THeartGraph, UHeartGraph>::IsDerived, "THeartGraph must derive from UHeartGraph");
		return Get(THeartGraph::StaticClass());
	}

	template <typename THeartGraphSchema>
	static const THeartGraphSchema* Get(const TSubclassOf<UHeartGraph> GraphClass)
	{
		static_assert(TIsDerivedFrom<THeartGraphSchema, UHeartGraphSchema>::IsDerived, "THeartGraphSchema must derive from UHeartGraphSchema");
		return Cast<THeartGraphSchema>(Get(GraphClass));
	}

	template <typename THeartGraphSchema, typename THeartGraph>
	static const THeartGraphSchema* Get()
	{
		static_assert(TIsDerivedFrom<THeartGraphSchema, UHeartGraphSchema>::IsDerived, "THeartGraphSchema must derive from UHeartGraphSchema");
		return Cast<THeartGraphSchema>(Get<THeartGraph>());
	}

protected:
	// Called by UHeartGraph's PreSave.
	virtual void OnPreSaveGraph(UHeartGraph* HeartGraph, const FObjectPreSaveContext& SaveContext) const;


public:
#if WITH_EDITOR
	bool GetRunCanPinsConnectInEdGraph() const { return RunCanPinsConnectInEdGraph; }

	static FName DefaultEditorStylePropertyName() { return GET_MEMBER_NAME_CHECKED(UHeartGraphSchema, DefaultEditorStyle); }
	FName GetDefaultEditorStyle() const { return DefaultEditorStyle; }
#endif

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|Schema")
	bool TryGetWorldForGraph(const UHeartGraph* HeartGraph, UWorld*& World) const;

public:
	// Get the class used by the HeartRegistryRuntimeSubsystem to track available nodes and visualizers for this graph.
	// This usually does not need to be implemented, as the default has most behavior setup out of the box.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|Schema")
	TSubclassOf<UHeartGraphNodeRegistry> GetRegistryClass() const;

	// @todo this visualizer stuff should absolutely not be part of UHeartGraphSchema. what if we wanted to visualize the same graph in multiple ways?
	// @todo maybe make a interface, or base class for visualizers, so this isn't just a UObject pointer?

	UFUNCTION(BlueprintCallable, Category = "Heart|Schema")
	UObject* GetConnectionVisualizer() const;

	template <typename TConnectionVisualizer>
	TConnectionVisualizer* GetConnectionVisualizer() const
	{
		return Cast<TConnectionVisualizer>(GetConnectionVisualizer());
	}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|Schema")
	UClass* GetConnectionVisualizerClass() const;


	UFUNCTION(BlueprintCallable, BlueprintPure = false, BlueprintNativeEvent, Category = "Heart|Schema")
	bool TryConnectPins(UHeartGraph* Graph, FHeartGraphPinReference PinA, FHeartGraphPinReference PinB) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|Schema")
	FHeartConnectPinsResponse CanPinsConnect(const UHeartGraph* Graph, FHeartGraphPinReference PinA, FHeartGraphPinReference PinB) const;

	// AKA, setup function called on all graphs when they are created.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|Schema")
	void CreateDefaultNodesForGraph(UHeartGraph* Graph) const;

protected:
	// These extensions are copied to each asset instance. They can be edited at runtime, but are otherwise, identical
	// for every instance of the graph class bound to this schema.
	UPROPERTY(EditAnywhere, Instanced, Category = "Extensions")
	TArray<TObjectPtr<UHeartGraphExtension>> DefaultExtensions;

	// These extension classes are used to add extensions to each asset instance. These instances are uniquely
	// customizable per instance, as only the existence of these classes is validated against.
	UPROPERTY(EditAnywhere, Category = "Extensions")
	TArray<TSubclassOf<UHeartGraphExtension>> AdditionalExtensionClasses;

#if WITH_EDITORONLY_DATA
	// Enable to have the runtime function CanPinsConnect called by the EdGraphSchema for this graph.
	UPROPERTY(EditAnywhere, Category = "Editor")
	bool RunCanPinsConnectInEdGraph;

	// Optimization to discard Graph Nodes at runtime. Enable this if the Heart Graph is used only as an
	// intermediate form, from which the Editor generates standalone data, and the node data is no longer used.
	UPROPERTY(EditAnywhere, Category = "Editor")
	bool FlushNodesForRuntime = false;

	// Style of slate widget to use by default
	UPROPERTY(EditAnywhere, Category = "Editor")
	FName DefaultEditorStyle;

	// Action to run on the graph during PreSave
	UPROPERTY(EditAnywhere, Category = "Editor")
	TSubclassOf<UHeartGraphAction> EditorPreSaveAction;
#endif
};
