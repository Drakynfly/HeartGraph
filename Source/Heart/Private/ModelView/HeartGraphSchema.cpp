// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartGraphSchema.h"

#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "Model/HeartGraph.h"
#include "ModelView/Actions/HeartGraphAction.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphSchema)

UHeartGraphSchema::UHeartGraphSchema()
{
#if WITH_EDITORONLY_DATA
	DefaultEditorStyle = "Horizontal";
#endif
}

const UHeartGraphSchema* UHeartGraphSchema::Get(const TSubclassOf<UHeartGraph>& GraphClass)
{
	if (!ensure(IsValid(GraphClass)))
	{
		return GetDefault<UHeartGraphSchema>();
	}

	const UHeartGraph* DefaultHeartGraph = GetDefault<UHeartGraph>(GraphClass);
	UClass* Class = DefaultHeartGraph->GetSchemaClass();

	if (!ensure(IsValid(Class)))
	{
		UE_LOG(LogHeartGraph, Warning, TEXT("GetSchemaClass for Graph Class '%s' returned nullptr!"), *GraphClass->GetName())
		return GetDefault<UHeartGraphSchema>();
	}

	return GetDefault<UHeartGraphSchema>(Class);
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

	// @todo this function needs to be rewritten if we want the extensions to be editable on a per graph class basis

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

	// If we are not running on a CDO or trying to cook, execute the EditorPreSaveAction
	if (!IsTemplate() && !SaveContext.IsCooking())
	{
		if (IsValid(EditorPreSaveAction))
		{
			UHeartActionBase::QuickExecuteGraphAction(EditorPreSaveAction, HeartGraph, FHeartManualEvent(0.0));
		}
	}
#endif
}

bool UHeartGraphSchema::TryConnectPins_Implementation(UHeartGraph* Graph, const FHeartGraphPinReference PinA, const FHeartGraphPinReference PinB) const
{
	bool bModified = false;

	switch (CanPinsConnect(Graph, PinA, PinB).Response)
	{
	case EHeartCanConnectPinsResponse::Allow:
		bModified |= Graph->EditConnections()
			.Connect(PinA, PinB)
			.Modified();
		break;

	case EHeartCanConnectPinsResponse::AllowBreakA:
		bModified |= Graph->EditConnections().
			DisconnectAll(PinA).
			Connect(PinA, PinB)
			.Modified();
		break;

	case EHeartCanConnectPinsResponse::AllowBreakB:
		bModified |= Graph->EditConnections()
			.DisconnectAll(PinB)
			.Connect(PinA, PinB)
			.Modified();
		break;

	case EHeartCanConnectPinsResponse::AllowBreakAB:
		bModified |= Graph->EditConnections()
			.DisconnectAll(PinA)
			.DisconnectAll(PinB)
			.Connect(PinA, PinB)
			.Modified();
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