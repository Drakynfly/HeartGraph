// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartGraphSchema.h"

#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "Location/Heart2DLocationComponent.h"
#include "Location/HeartNodeLocationComponentBase.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphExtension.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartPinConnectionEdit.h"
#include "ModelView/Actions/HeartGraphAction.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphSchema)

UHeartGraphSchema::UHeartGraphSchema()
{
	LocationComponentClass = UHeart2DLocationComponent::StaticClass();

#if WITH_EDITORONLY_DATA
	DefaultEditorStyle = "Horizontal";
#endif
}

bool UHeartGraphSchema::TryGetWorldForGraph_Implementation(const UHeartGraph* HeartGraph, UWorld*& World) const
{
	return false;
}

TSubclassOf<UHeartGraphNodeRegistry> UHeartGraphSchema::GetRegistryClass_Implementation() const
{
	return UHeartGraphNodeRegistry::StaticClass();
}

void UHeartGraphSchema::InitializeNewGraph(UHeartGraph* HeartGraph) const
{
	HeartGraph->SchemaClass = GetClass();
	RefreshGraphExtensions(HeartGraph);
	CreateDefaultNodesForGraph(HeartGraph);
}

void UHeartGraphSchema::OnPreSaveGraph(UHeartGraph* HeartGraph, const FObjectPreSaveContext& SaveContext) const
{
	if (!ensure(HeartGraph)) return;

#if WITH_EDITOR
	RefreshGraphExtensions(HeartGraph);

	// If we are not running on a CDO or trying to cook, execute the EditorPreSaveAction
	if (!SaveContext.IsCooking())
	{
		if (IsValid(EditorPreSaveAction))
		{
			Heart::Action::Execute(EditorPreSaveAction, HeartGraph, FHeartManualEvent(0.0));
		}
	}
#endif
}

void UHeartGraphSchema::RefreshGraphExtensions(UHeartGraph* HeartGraph) const
{
	// Update Node Location Component
	{
		UClass* CurrentClass = nullptr;
		if (const UHeartNodeLocationComponentBase* LocationInterface = HeartGraph->NodeLocationComponent)
		{
			CurrentClass = LocationInterface->GetClass();
		}

		if (CurrentClass != LocationComponentClass)
		{
			if (IsValid(LocationComponentClass))
			{
				UHeartNodeLocationComponentBase* OldComponent = HeartGraph->NodeLocationComponent;
				HeartGraph->NodeLocationComponent = NewObject<UHeartNodeLocationComponentBase>(HeartGraph, LocationComponentClass);
				HeartGraph->NodeLocationComponent->PostComponentAdded();
				if (OldComponent)
				{
					FInstancedStruct MigrationData;
					OldComponent->ExportMigrationData(MigrationData);
					HeartGraph->NodeLocationComponent->ImportMigrationData(MigrationData);
				}
				else
				{
					PRAGMA_DISABLE_DEPRECATION_WARNINGS
					for (auto&& Element : HeartGraph->GetNodes())
					{
						HeartGraph->NodeLocationComponent->SetNodeLocation(Element.Value->GetGuid(), Element.Value->GetLocation(), false);
					}
					PRAGMA_ENABLE_DEPRECATION_WARNINGS
				}
			}
			else
			{
				HeartGraph->NodeLocationComponent = nullptr;
			}
		}
	}

	// Update Extensions Map
	{
		// Reset the "all extensions" map.
		HeartGraph->Extensions.Empty(DefaultExtensions.Num() + HeartGraph->InstancedExtensions.Num());

		// Add the Schema Extensions.
		for (auto&& Extension : DefaultExtensions)
		{
			if (!IsValid(Extension))
			{
				continue;
			}

			HeartGraph->AddExtensionInstance(DuplicateObject(Extension, HeartGraph));
		}

		// Add the Graph Extensions.
		for (auto&& Extension : HeartGraph->InstancedExtensions)
		{
			if (!IsValid(Extension))
			{
				continue;
			}

			HeartGraph->AddExtensionInstance(Extension);
		}
	}
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
#if !UE_BUILD_SHIPPING
	return FHeartConnectPinsResponse{EHeartCanConnectPinsResponse::Allow, FText::FromString("Allowed by base implementation")};
#else
	return FHeartConnectPinsResponse{EHeartCanConnectPinsResponse::Allow};
#endif
}

void UHeartGraphSchema::CreateDefaultNodesForGraph_Implementation(UHeartGraph* Graph) const
{
	// Does nothing by default
}