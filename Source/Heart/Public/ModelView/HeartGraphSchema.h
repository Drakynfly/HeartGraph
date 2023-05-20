// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGraphSchema.generated.h"

struct FHeartGraphPinReference;
class UHeartGraph;
class UHeartGraphNodeRegistry;

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
};

/**
 * Base class for Heart "Schemas", const classes that are interacted with via only their CDO to describe the behavior
 * of a Heart Graph instance.
 */
UCLASS(Abstract, Const, BlueprintType, Blueprintable)
class HEART_API UHeartGraphSchema : public UObject // Based on UEdGraphSchema
{
	GENERATED_BODY()

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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|Schema")
	bool TryGetWorldForGraph(const UHeartGraph* HeartGraph, UWorld*& World) const;

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
#endif
};
