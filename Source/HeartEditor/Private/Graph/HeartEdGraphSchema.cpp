// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartEdGraphSchema.h"

#include "Graph/HeartGraphAssetEditor.h"
#include "Graph/HeartEdGraphSchema_Actions.h"
#include "Graph/HeartGraphUtils.h"
#include "Nodes/HeartEdGraphNode.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "ModelView/HeartGraphSchema.h"

#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "EdGraph/EdGraph.h"
#include "ScopedTransaction.h"
#include "Graph/HeartEdGraph.h"

#define LOCTEXT_NAMESPACE "HeartGraphSchema"

void UHeartEdGraphSchema::GetPaletteActions(FGraphActionMenuBuilder& ActionMenuBuilder, const UClass* AssetClass, const FString& CategoryName)
{
	GetHeartGraphNodeActions(ActionMenuBuilder, AssetClass->GetDefaultObject<UHeartGraph>(), CategoryName);
	GetCommentAction(ActionMenuBuilder);
}

void UHeartEdGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	GetHeartGraphNodeActions(ContextMenuBuilder, GetAssetClassDefaults(ContextMenuBuilder.CurrentGraph), FString());
	GetCommentAction(ContextMenuBuilder, ContextMenuBuilder.CurrentGraph);

	if (!ContextMenuBuilder.FromPin && Heart::GraphUtils::GetHeartGraphAssetEditor(ContextMenuBuilder.CurrentGraph)->CanPasteNodes())
	{
		const TSharedPtr<FHeartGraphSchemaAction_Paste> NewAction(new FHeartGraphSchemaAction_Paste(FText::GetEmpty(), LOCTEXT("PasteHereAction", "Paste here"), FText::GetEmpty(), 0));
		ContextMenuBuilder.AddAction(NewAction);
	}
}

const FPinConnectionResponse UHeartEdGraphSchema::CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	auto&& OwningNodeA = Cast<UHeartEdGraphNode>(PinA->GetOwningNodeUnchecked());
	auto&& OwningNodeB = Cast<UHeartEdGraphNode>(PinB->GetOwningNodeUnchecked());

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

	auto&& RuntimeSchema = OwningNodeA->GetHeartGraphNode()->GetGraph()->GetSchema();

	if (ensure(IsValid(RuntimeSchema)))
	{
		if (RuntimeSchema->RunCanPinsConnectInEdGraph)
		{
			auto&& HeartPinA = OwningNodeA->GetPinByName(PinA->GetFName());
			auto&& HeartPinB = OwningNodeB->GetPinByName(PinB->GetFName());

			FHeartConnectPinsResponse RuntimeResult;

			{
				// Run blueprint logic to see if pins are compatible
				FEditorScriptExecutionGuard EditorScriptExecutionGuard;
				RuntimeResult = RuntimeSchema->CanPinsConnect(HeartPinA, HeartPinB);
			}

			FPinConnectionResponse Response;
			Response.Response = static_cast<ECanCreateConnectionResponse>(RuntimeResult.Response);
			Response.Message = RuntimeResult.Message;
			return Response;
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
	const bool bModified = UEdGraphSchema::TryCreateConnection(PinA, PinB);

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
	return TSharedPtr<FEdGraphSchemaAction>(static_cast<FEdGraphSchemaAction*>(new FHeartGraphSchemaAction_NewComment));
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
	Super::CreateDefaultNodesForGraph(Graph);

	auto&& HeartEdGraph = Cast<UHeartEdGraph>(&Graph);
	check(HeartEdGraph);

	auto&& HeartGraph = HeartEdGraph->GetHeartGraph();
	check(HeartGraph);

	auto&& HeartSchema = HeartGraph->GetSchema();
	check(HeartSchema);

	HeartSchema->CreateDefaultNodesForGraph(HeartGraph);
}

TArray<TSharedPtr<FString>> UHeartEdGraphSchema::GetHeartGraphNodeCategories(TSubclassOf<UHeartGraph> HeartGraphClass)
{
	auto&& Registry = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->GetRegistry(HeartGraphClass);
	auto&& SortedCategories = Registry->GetNodeCategories();

	// create list of categories
	TArray<TSharedPtr<FString>> Result;
	Result.Reserve(SortedCategories.Num());
	for (const FString& Category : SortedCategories)
	{
		if (!Category.IsEmpty())
		{
			Result.Emplace(MakeShareable(new FString(Category)));
		}
	}

	return Result;
}

UClass* UHeartEdGraphSchema::GetAssignedEdGraphNodeClass(const UClass* HeartGraphNodeClass)
{
	/*
	if (UClass* AssignedGraphNode = AssignedGraphNodeClasses.FindRef(HeartGraphNodeClass))
	{
		return AssignedGraphNode;
	}
	*/

	return UHeartEdGraphNode::StaticClass();
}

void UHeartEdGraphSchema::GetHeartGraphNodeActions(FGraphActionMenuBuilder& ActionMenuBuilder, const UHeartGraph* AssetClassDefaults, const FString& CategoryName)
{
	auto&& Registry = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->GetRegistry(AssetClassDefaults->GetClass());

	Registry->ForEachNodeObjectClass(
		[&CategoryName, &ActionMenuBuilder](const TSubclassOf<UHeartGraphNode> GraphNodeClass, const UClass* NodeClass)
		{
			auto&& GraphNodeDefault = GetDefault<UHeartGraphNode>(GraphNodeClass);

			if (ensure(IsValid(GraphNodeDefault)))
			{
				if (GraphNodeDefault->CanCreate_Editor())
				{
					if (CategoryName.IsEmpty() ||
						CategoryName.Equals(GraphNodeDefault->GetDefaultNodeCategory(NodeClass).ToString()))
					{
						const TSharedPtr<FHeartGraphSchemaAction_NewNode> NewNodeAction(new FHeartGraphSchemaAction_NewNode(NodeClass, GraphNodeDefault));
						ActionMenuBuilder.AddAction(NewNodeAction);
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

		const TSharedPtr<FHeartGraphSchemaAction_NewComment> NewAction(new FHeartGraphSchemaAction_NewComment(FText::GetEmpty(), MenuDescription, ToolTip, 0));
		ActionMenuBuilder.AddAction(NewAction);
	}
}

bool UHeartEdGraphSchema::IsHeartGraphNodePlaceable(const UClass* Class)
{
	if (Class->HasAnyClassFlags(CLASS_Abstract | CLASS_NotPlaceable | CLASS_Deprecated))
	{
		return false;
	}

	return true;
}

UBlueprint* UHeartEdGraphSchema::GetPlaceableNodeBlueprint(const FAssetData& AssetData)
{
	UBlueprint* Blueprint = Cast<UBlueprint>(AssetData.GetAsset());
	if (Blueprint && IsHeartGraphNodePlaceable(Blueprint->GeneratedClass))
	{
		return Blueprint;
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
