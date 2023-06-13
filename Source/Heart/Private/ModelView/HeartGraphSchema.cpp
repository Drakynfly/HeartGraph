// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartGraphSchema.h"

#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "ModelView/Actions/HeartGraphAction.h"
#include "UI/HeartInputActivation.h"
#include "UObject/ObjectSaveContext.h"

UHeartGraphSchema::UHeartGraphSchema()
{
#if WITH_EDITORONLY_DATA
	DefaultEditorStyle = "Horizontal";
#endif
}

const UHeartGraphSchema* UHeartGraphSchema::Get(const TSubclassOf<UHeartGraph> GraphClass)
{
	if (!ensure(IsValid(GraphClass)))
	{
		return GetDefault<UHeartGraphSchema>();
	}

	const UHeartGraph* DefaultHeartGraph = GetDefault<UHeartGraph>(GraphClass);
	const UClass* Class = DefaultHeartGraph->GetSchemaClass();

	if (!ensure(IsValid(Class)))
	{
		UE_LOG(LogHeartGraph, Warning, TEXT("GetSchemaClass for Graph Class '%s' returned nullptr!"), *GraphClass->GetName())
		return GetDefault<UHeartGraphSchema>();
	}

	return GetDefault<UHeartGraphSchema>(DefaultHeartGraph->GetSchemaClass());
}

bool UHeartGraphSchema::TryGetWorldForGraph_Implementation(const UHeartGraph* HeartGraph, UWorld*& World) const
{
	return false;
}

TSubclassOf<UHeartGraphNodeRegistry> UHeartGraphSchema::GetRegistryClass_Implementation() const
{
	return UHeartGraphNodeRegistry::StaticClass();
}

void UHeartGraphSchema::OnPreSaveGraph(UHeartGraph* HeartGraph, const FObjectPreSaveContext& SaveContext) const
{
	if (!ensure(HeartGraph)) return;

#if WITH_EDITOR
	// Reset instance extensions.
	TSet<TSubclassOf<UHeartGraphExtension>> PreviousClassList;
	HeartGraph->Extensions.GetKeys(PreviousClassList);

	for (auto&& Extension : DefaultExtensions)
	{
		PreviousClassList.Remove(Extension->GetClass());
		HeartGraph->RemoveExtension(Extension->GetClass());
		HeartGraph->AddExtensionInstance(DuplicateObject(Extension, HeartGraph));
	}

	for (auto&& ExtensionClass : AdditionalExtensionClasses)
	{
		// Add an extension of this class if none exist
		PreviousClassList.Remove(ExtensionClass);
		HeartGraph->AddExtension(ExtensionClass);
	}

	// Cleanup extensions for classes no longer registered
	for (auto&& Class : PreviousClassList)
	{
		HeartGraph->RemoveExtension(Class);
	}

	if (IsValid(EditorPreSaveAction))
	{
		UHeartGraphActionBase::QuickExecuteGraphAction(EditorPreSaveAction, HeartGraph, FHeartManualEvent(0.0));
	}
#endif
}

UObject* UHeartGraphSchema::GetConnectionVisualizer() const
{
	if (auto&& Class = GetConnectionVisualizerClass())
	{
		return Class->GetDefaultObject();
	}

	return nullptr;
}

bool UHeartGraphSchema::TryConnectPins_Implementation(UHeartGraph* Graph, FHeartGraphPinReference PinA, FHeartGraphPinReference PinB) const
{
	UHeartGraphNode* NodeA = Graph->GetNode(PinA.NodeGuid);
	UHeartGraphNode* NodeB = Graph->GetNode(PinB.NodeGuid);

	const FHeartConnectPinsResponse Response = CanPinsConnect(Graph, PinA, PinB);

	bool bModified = false;

	switch (Response.Response)
	{
	case EHeartCanConnectPinsResponse::Allow:
		Graph->ConnectPins(PinA, PinB);
		bModified = true;
		break;

	case EHeartCanConnectPinsResponse::AllowBreakA:
		Graph->DisconnectAllPins(PinA);
		Graph->ConnectPins(PinA, PinB);
		bModified = true;
		break;

	case EHeartCanConnectPinsResponse::AllowBreakB:
		Graph->DisconnectAllPins(PinB);
		Graph->ConnectPins(PinA, PinB);
		bModified = true;
		break;

	case EHeartCanConnectPinsResponse::AllowBreakAB:
		Graph->DisconnectAllPins(PinA);
		Graph->DisconnectAllPins(PinB);
		Graph->ConnectPins(PinA, PinB);
		bModified = true;
		break;

	/**
	case CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE:
		bModified = CreateAutomaticConversionNodeAndConnections(PinA, PinB);
		break;

	case CONNECT_RESPONSE_MAKE_WITH_PROMOTION:
		bModified = CreatePromotedConnection(PinA, PinB);
		break;
	*/

	case EHeartCanConnectPinsResponse::Disallow:
	default:
		break;
	}

	if (bModified)
	{
		NodeA->NotifyPinConnectionsChanged(PinA.PinGuid);
		NodeB->NotifyPinConnectionsChanged(PinB.PinGuid);
		Graph->NotifyNodeConnectionsChanged({NodeA, NodeB}, {PinA.PinGuid, PinB.PinGuid});
	}

	return bModified;
}
FHeartConnectPinsResponse UHeartGraphSchema::CanPinsConnect_Implementation(const UHeartGraph* Graph, FHeartGraphPinReference PinA, FHeartGraphPinReference PinB) const
{
	return FHeartConnectPinsResponse{EHeartCanConnectPinsResponse::Allow};
}

void UHeartGraphSchema::CreateDefaultNodesForGraph_Implementation(UHeartGraph* Graph) const
{
	// Does nothing by default
}

UClass* UHeartGraphSchema::GetConnectionVisualizerClass_Implementation() const
{
	return nullptr;
}
