// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartEdGraphSchema.h"

#include "Graph/HeartGraphAssetEditor.h"
#include "Graph/HeartEdGraphSchema_Actions.h"
#include "Graph/HeartEdGraphUtils.h"
#include "Nodes/HeartEdGraphNode.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "ModelView/HeartGraphSchema.h"

#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"

#include "EdGraph/EdGraph.h"
#include "Graph/HeartEdGraph.h"

#include "ScopedTransaction.h"
#include "Input/HeartInputLinkerBase.h"
#include "Input/HeartSlateInputLinker.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartEdGraphSchema)

#define LOCTEXT_NAMESPACE "HeartGraphSchema"

namespace Heart::Editor
{
	// Converts a Runtime Call response struct into a editor response struct
	FPinConnectionResponse ConvertConnectPinsResponseToPinConnectionResponse(const FHeartConnectPinsResponse& RuntimeResponse)
	{
		FPinConnectionResponse Response;
		Response.Message = RuntimeResponse.Message;
		Response.Response = static_cast<ECanCreateConnectionResponse>(RuntimeResponse.Response);
		return Response;
	}
}

void UHeartEdGraphSchema::GetPaletteActions(FGraphActionMenuBuilder& ActionMenuBuilder, const UHeartGraph* GraphAsset, const TOptional<FStringView>& CategoryName)
{
	GetHeartGraphNodeActions(ActionMenuBuilder, GraphAsset, CategoryName);

	if (!CategoryName.IsSet())
	{
		GetCommentAction(ActionMenuBuilder);
	}
}

void UHeartEdGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	GetHeartGraphNodeActions(ContextMenuBuilder, GetAssetFromEdGraph(ContextMenuBuilder.CurrentGraph), {});
	GetCommentAction(ContextMenuBuilder, ContextMenuBuilder.CurrentGraph);

	auto GraphEditor = Heart::GraphUtils::GetHeartGraphAssetEditor(ContextMenuBuilder.CurrentGraph);
	if (!GraphEditor.IsValid())
	{
		return;
	}

	/** Paste Action **/

	if (!ContextMenuBuilder.FromPin && GraphEditor->CanPasteNodes())
	{
		const TSharedPtr<FHeartGraphSchemaAction_Paste> NewAction =
			MakeShared<FHeartGraphSchemaAction_Paste>(FText::GetEmpty(), LOCTEXT("PasteHereAction", "Paste here"), FText::GetEmpty(), 0);
		ContextMenuBuilder.AddAction(NewAction);
	}

	/** Linker Actions **/

	auto&& HeartEdGraph = CastChecked<UHeartEdGraph>(ContextMenuBuilder.CurrentGraph);
	auto&& Linker = HeartEdGraph->GetEditorLinker();
	if (IsValid(Linker))
	{
		TArray<FHeartManualInputQueryResult> QueryResults = Linker->QueryManualTriggers(HeartEdGraph);

		for (auto&& QueryResult : QueryResults)
		{
			auto NewAction = MakeShared<FHeartGraphSchemaAction_LinkerBinding>(
				LOCTEXT("LinkerActions", "Linker Actions"),
				QueryResult.Description,
				FText::GetEmpty(),
				11,
				QueryResult.Key);

			ContextMenuBuilder.AddAction(NewAction);
		}
	}
}

const FPinConnectionResponse UHeartEdGraphSchema::CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	const UHeartEdGraphNode* OwningNodeA = Cast<UHeartEdGraphNode>(PinA->GetOwningNodeUnchecked());
	const UHeartEdGraphNode* OwningNodeB = Cast<UHeartEdGraphNode>(PinB->GetOwningNodeUnchecked());

	const UHeartGraph* Graph = OwningNodeA->GetHeartGraphNode()->GetGraph();

	check(OwningNodeB->GetHeartGraphNode()->GetGraph() == Graph);

	if (!OwningNodeA || !OwningNodeB)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Invalid nodes"));
	}

	// Make sure the pins are not on the same node
	if (PinA->GetOwningNode() == PinB->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Both are on the same node"));
	}

	if (PinA->bOrphanedPin || PinB->bOrphanedPin)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Cannot make new connections to orphaned pin"));
	}

	if (const UHeartGraphSchema* RuntimeSchema = OwningNodeA->GetHeartGraphNode()->GetGraph()->GetSchema();
		ensure(IsValid(RuntimeSchema)))
	{
		if (RuntimeSchema->GetRunCanPinsConnectInEdGraph())
		{
			auto&& HeartNodeA = OwningNodeA->GetHeartGraphNode();
			auto&& HeartNodeB = OwningNodeB->GetHeartGraphNode();

			if (!ensure(HeartNodeA && HeartNodeB))
			{
				return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Invalid Heart Graph Node!"));
			}

			const FHeartPinGuid& HeartPinA = HeartNodeA->GetPinByName(PinA->GetFName());
			const FHeartPinGuid& HeartPinB = HeartNodeB->GetPinByName(PinB->GetFName());

			FHeartConnectPinsResponse RuntimeResult;

			{
				// Run blueprint logic to see if pins are compatible
				FEditorScriptExecutionGuard EditorScriptExecutionGuard;
				RuntimeResult = RuntimeSchema->CanPinsConnect(Graph, {HeartNodeA->GetGuid(), HeartPinA}, {HeartNodeB->GetGuid(), HeartPinB});
			}

			return Heart::Editor::ConvertConnectPinsResponseToPinConnectionResponse(RuntimeResult);
		}
	}

	/// vvv Default Pin Connection Logic if RunCanPinsConnectInEdGraph is disabled vvv

	// Compare the directions
	const UEdGraphPin* InputPin = nullptr;
	const UEdGraphPin* OutputPin = nullptr;

	if (!CategorizePinsByDirection(PinA, PinB, /*out*/ InputPin, /*out*/ OutputPin))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Directions are not compatible"));
	}

	// Break existing connections on outputs only - multiple input connections are acceptable
	if (OutputPin->LinkedTo.Num() > 0)
	{
		const ECanCreateConnectionResponse ReplyBreakInputs = (OutputPin == PinA ? CONNECT_RESPONSE_BREAK_OTHERS_A : CONNECT_RESPONSE_BREAK_OTHERS_B);
		return FPinConnectionResponse(ReplyBreakInputs, TEXT("Replace existing connections"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

bool UHeartEdGraphSchema::TryCreateConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	const bool bModified = Super::TryCreateConnection(PinA, PinB);

	if (bModified)
	{
		PinA->GetOwningNode()->GetGraph()->NotifyGraphChanged();
	}

	return bModified;
}

bool UHeartEdGraphSchema::ShouldHidePinDefaultValue(UEdGraphPin* Pin) const
{
	return true;
}

FLinearColor UHeartEdGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::White;
}

void UHeartEdGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	Super::BreakNodeLinks(TargetNode);

	TargetNode.GetGraph()->NotifyGraphChanged();
}

void UHeartEdGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const
{
	const FScopedTransaction Transaction(LOCTEXT("GraphEd_BreakPinLinks", "Break Pin Links"));

	Super::BreakPinLinks(TargetPin, bSendsNodeNotification);

	if (TargetPin.bOrphanedPin)
	{
		// this calls NotifyGraphChanged()
		Cast<UHeartEdGraphNode>(TargetPin.GetOwningNode())->RemoveOrphanedPin(&TargetPin);
	}
	else if (bSendsNodeNotification)
	{
		TargetPin.GetOwningNode()->GetGraph()->NotifyGraphChanged();
	}
}

int32 UHeartEdGraphSchema::GetNodeSelectionCount(const UEdGraph* Graph) const
{
	return Heart::GraphUtils::GetHeartGraphAssetEditor(Graph)->GetNumberOfSelectedNodes();
}

TSharedPtr<FEdGraphSchemaAction> UHeartEdGraphSchema::GetCreateCommentAction() const
{
	return MakeShared<FHeartGraphSchemaAction_NewComment>();
}

void UHeartEdGraphSchema::OnPinConnectionDoubleCicked(UEdGraphPin* PinA, UEdGraphPin* PinB,
	const FVector2D& GraphPosition) const
{
	OnPinConnectionDoubleClicked(PinA, PinB, GraphPosition);
}

void UHeartEdGraphSchema::OnPinConnectionDoubleClicked(UEdGraphPin* PinA, UEdGraphPin* PinB,
	const FVector2D& GraphPosition) const
{
	Super::OnPinConnectionDoubleCicked(PinA, PinB, GraphPosition);
}

void UHeartEdGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	// We don't need to create anything here.
	// Nodes that are supposed to be added on asset creation are handled by InitializeNewGraph, which is called by UHeartGraphFactory::FactoryCreateNew
}

TArray<TSharedPtr<FString>> UHeartEdGraphSchema::GetHeartGraphNodeCategories(const UHeartGraph* GraphAsset)
{
	auto&& Registry = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->GetNodeRegistry(GraphAsset->GetSchema()->GetClass());
	auto&& SortedCategories = Registry->GetNodeCategories();

	// create list of categories
	TArray<TSharedPtr<FString>> Result;
	Result.Reserve(SortedCategories.Num());
	for (const FString& Category : SortedCategories)
	{
		Result.Emplace(MakeShared<FString>(Category));
	}

	return Result;
}

void UHeartEdGraphSchema::GetHeartGraphNodeActions(FGraphActionMenuBuilder& ActionMenuBuilder, const UHeartGraph* GraphAsset, const TOptional<FStringView>& CategoryName)
{
	auto&& Registry = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->GetNodeRegistry(GraphAsset->GetSchema()->GetClass());

	Registry->ForEachNodeObjectClass(
		[&CategoryName, &ActionMenuBuilder](const FHeartNodeArchetype& Archetype)
		{
			if (Archetype.Source.ThisClass()->HasAnyClassFlags(CLASS_Abstract)) return true;

			auto&& GraphNodeDefault = GetDefault<UHeartGraphNode>(Archetype.GraphNode);

			if (ensure(IsValid(GraphNodeDefault)))
			{
				if (GraphNodeDefault->CanCreate_Editor())
				{
					if (!CategoryName.IsSet() ||
						CategoryName.GetValue() == GraphNodeDefault->GetDefaultNodeCategory(Archetype.Source).ToString())
					{
						ActionMenuBuilder.AddAction(MakeShared<FHeartGraphSchemaAction_NewNode>(Archetype));
					}
				}
			}

			// Always continue iterating...
			return true;
		});
}

void UHeartEdGraphSchema::GetCommentAction(FGraphActionMenuBuilder& ActionMenuBuilder, const UEdGraph* CurrentGraph /*= nullptr*/)
{
	if (!ActionMenuBuilder.FromPin)
	{
		const bool bIsManyNodesSelected = CurrentGraph ? (Heart::GraphUtils::GetHeartGraphAssetEditor(CurrentGraph)->GetNumberOfSelectedNodes() > 0) : false;
		const FText MenuDescription = bIsManyNodesSelected ? LOCTEXT("CreateCommentAction", "Create Comment from Selection") : LOCTEXT("AddCommentAction", "Add Comment...");
		const FText ToolTip = LOCTEXT("CreateCommentToolTip", "Creates a comment.");

		ActionMenuBuilder.AddAction(MakeShared<FHeartGraphSchemaAction_NewComment>(FText::GetEmpty(), MenuDescription, ToolTip, 0));
	}
}

const UHeartGraph* UHeartEdGraphSchema::GetAssetFromEdGraph(const UEdGraph* Graph)
{
	if (Graph)
	{
		if (const UHeartGraph* HeartGraph = Graph->GetTypedOuter<UHeartGraph>())
		{
			return HeartGraph;
		}
	}

	return nullptr;
}

const UHeartGraph* UHeartEdGraphSchema::GetAssetClassDefaults(const UEdGraph* Graph)
{
	const UClass* AssetClass = UHeartGraph::StaticClass();

	if (Graph)
	{
		if (const UHeartGraph* HeartGraph = Graph->GetTypedOuter<UHeartGraph>())
		{
			AssetClass = HeartGraph->GetClass();
		}
	}

	return AssetClass->GetDefaultObject<UHeartGraph>();
}

#undef LOCTEXT_NAMESPACE