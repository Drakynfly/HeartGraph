// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/Widgets/Nodes/SHeartGraphNodeBase.h"
#include "Graph/Widgets/Pins/SHeartGraphPin.h"

#include "HeartEditorStyle.h"

#include "Model/HeartGraphNode.h"

#include "EdGraph/EdGraphPin.h"
#include "Editor.h"
#include "GraphEditorSettings.h"
#include "IDocumentation.h"
#include "Input/Reply.h"
#include "Layout/Margin.h"
#include "Misc/Attribute.h"
#include "SCommentBubble.h"
#include "SGraphNode.h"
#include "SNodePanel.h"
#include "Graph/HeartEdGraph.h"
#include "Slate/SHeartGraphWidgetBase.h"
#include "Styling/SlateColor.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "SHeartGraphNodeBase"

void SHeartGraphNodeBase::Construct(const FArguments& InArgs, UHeartEdGraphNode* InNode)
{
	GraphNode = InNode;
	HeartEdGraphNode = InNode;

	auto EditorLinker = CastChecked<UHeartEdGraph>(InNode->GetGraph())->GetEditorLinker();
	check(EditorLinker);

	AddMetadata(MakeShared<Heart::Canvas::FNodeAndLinkerMetadata>(
		HeartEdGraphNode->GetHeartGraphNode(), EditorLinker));

	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

void SHeartGraphNodeBase::GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const
{
	TArray<TPair<FString, FLinearColor>> PopupMessages;
	HeartEdGraphNode->GetPopupMessages(PopupMessages);

	for (const TPair<FString, FLinearColor>& NodeMessage : PopupMessages)
	{
		const FSlateBrush* Icon = nullptr; // @todo provide interface to set these

		const FGraphInformationPopupInfo DescriptionPopup = FGraphInformationPopupInfo(Icon, NodeMessage.Value, NodeMessage.Key);
		Popups.Add(DescriptionPopup);
	}
}

const FSlateBrush* SHeartGraphNodeBase::GetShadowBrush(bool bSelected) const
{
	return SGraphNode::GetShadowBrush(bSelected);
}

void SHeartGraphNodeBase::GetOverlayBrushes(bool bSelected, const FVector2D WidgetSize, TArray<FOverlayBrushInfo>& Brushes) const
{
	// Node breakpoint
	if (HeartEdGraphNode->NodeBreakpoint.bHasBreakpoint)
	{
		FOverlayBrushInfo NodeBrush;

		if (HeartEdGraphNode->NodeBreakpoint.bBreakpointHit)
		{
			NodeBrush.Brush = FHeartEditorStyle::Get()->GetBrush(TEXT("HeartGraph.BreakpointHit"));
			NodeBrush.OverlayOffset.X = WidgetSize.X - 12.0f;
		}
		else
		{
			NodeBrush.Brush = FHeartEditorStyle::Get()->GetBrush(HeartEdGraphNode->NodeBreakpoint.bBreakpointEnabled ? TEXT("HeartGraph.BreakpointEnabled") : TEXT("HeartGraph.BreakpointDisabled"));
			NodeBrush.OverlayOffset.X = WidgetSize.X;
		}

		NodeBrush.OverlayOffset.Y = -NodeBrush.Brush->ImageSize.Y;
		NodeBrush.AnimationEnvelope = FVector2D(0.f, 10.f);
		Brushes.Add(NodeBrush);
	}

	// Pin breakpoints
	for (const TPair<FEdGraphPinReference, FHeartBreakpoint>& PinBreakpoint : HeartEdGraphNode->PinBreakpoints)
	{
		if (PinBreakpoint.Key.Get()->Direction == EGPD_Input)
		{
			GetPinBrush(true, WidgetSize.X, HeartEdGraphNode->InputPins.IndexOfByKey(PinBreakpoint.Key.Get()), PinBreakpoint.Value, Brushes);
		}
		else
		{
			GetPinBrush(false, WidgetSize.X, HeartEdGraphNode->OutputPins.IndexOfByKey(PinBreakpoint.Key.Get()), PinBreakpoint.Value, Brushes);
		}
	}
}

void SHeartGraphNodeBase::GetPinBrush(const bool bLeftSide, const float WidgetWidth, const int32 PinIndex, const FHeartBreakpoint& Breakpoint, TArray<FOverlayBrushInfo>& Brushes) const
{
	if (Breakpoint.bHasBreakpoint)
	{
		FOverlayBrushInfo PinBrush;

		if (Breakpoint.bBreakpointHit)
		{
			PinBrush.Brush = FHeartEditorStyle::Get()->GetBrush(TEXT("HeartGraph.PinBreakpointHit"));
			PinBrush.OverlayOffset.X = bLeftSide ? 0.0f : (WidgetWidth - 36.0f);
			PinBrush.OverlayOffset.Y = 12.0f + PinIndex * 28.0f;
		}
		else
		{
			PinBrush.Brush = FHeartEditorStyle::Get()->GetBrush(Breakpoint.bBreakpointEnabled ? TEXT("HeartGraph.BreakpointEnabled") : TEXT("HeartGraph.BreakpointDisabled"));
			PinBrush.OverlayOffset.X = bLeftSide ? -24.0f : WidgetWidth;
			PinBrush.OverlayOffset.Y = 16.0f + PinIndex * 28.0f;
		}

		PinBrush.AnimationEnvelope = FVector2D(0.f, 10.f);
		Brushes.Add(PinBrush);
	}
}

void SHeartGraphNodeBase::UpdateGraphNode()
{
	checkf(0, TEXT("UpdateGraphNode should be overriden by children"));
}

void SHeartGraphNodeBase::UpdateErrorInfo()
{
	if (auto&& HeartGraphNode = HeartEdGraphNode->GetHeartGraphNode())
	{
		if (HeartGraphNode->GetClass()->HasAnyClassFlags(CLASS_Deprecated))
		{
			ErrorMsg = FString(TEXT(" DEPRECATED! "));
			ErrorColor = FAppStyle::GetColor("ErrorReporting.WarningBackgroundColor");
			return;
		}
	}

	SGraphNode::UpdateErrorInfo();
}

TSharedRef<SWidget> SHeartGraphNodeBase::CreateTitleWidget(TSharedPtr<SNodeTitle> NodeTitle)
{
	SAssignNew(InlineEditableText, SInlineEditableTextBlock)
		.Style(FAppStyle::Get(), "Graph.Node.NodeTitleInlineEditableText")
		.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
		.OnVerifyTextChanged(this, &SHeartGraphNodeBase::OnVerifyNameTextChanged)
		.OnTextCommitted(this, &SHeartGraphNodeBase::OnNameTextCommited)
		.IsReadOnly(this, &SHeartGraphNodeBase::IsNameReadOnly)
		.IsSelected(this, &SHeartGraphNodeBase::IsSelectedExclusively);
	InlineEditableText->SetColorAndOpacity(TAttribute<FLinearColor>::Create(TAttribute<FLinearColor>::FGetter::CreateSP(this, &SHeartGraphNodeBase::GetNodeTitleTextColor)));

	return InlineEditableText.ToSharedRef();
}

TSharedRef<SWidget> SHeartGraphNodeBase::CreateNodeContentArea()
{
	checkf(0, TEXT("CreateNodeContentArea should be overriden by children"));
	return SNullWidget::NullWidget;
}

const FSlateBrush* SHeartGraphNodeBase::GetNodeBodyBrush() const
{
	return FHeartEditorStyle::GetBrush("Heart.Node.Body");
}

void SHeartGraphNodeBase::CreateStandardPinWidget(UEdGraphPin* Pin)
{
	const TSharedPtr<SGraphPin> NewPin = SNew(SHeartGraphPin, Pin)
		.Linker(GetMetaData<Heart::Canvas::FNodeAndLinkerMetadata>()->Linker.Get());

	const UHeartGraphNode* HeartGraphNode = HeartEdGraphNode->GetHeartGraphNode();

	if (IsValid(HeartGraphNode))
	{
		if (Pin->Direction == EGPD_Input)
		{
			//if (HeartGraphNode->GetInputPins().Num() == 1 && Pin->PinName == UHeartGraphNode::DefaultInputPin.PinName)
			{
			//	NewPin->SetShowLabel(false);
			}
		}
		else
		{
			//if (HeartGraphNode->GetOutputPins().Num() == 1 && Pin->PinName == UHeartGraphNode::DefaultOutputPin.PinName)
			{
			//	NewPin->SetShowLabel(false);
			}
		}
	}

	AddPin(NewPin.ToSharedRef());
}

TSharedPtr<SToolTip> SHeartGraphNodeBase::GetComplexTooltip()
{
	return IDocumentation::Get()->CreateToolTip(
		TAttribute<FText>(this, &SHeartGraphNodeBase::GetNodeTooltip),
		nullptr,
		GraphNode->GetDocumentationLink(),
		GraphNode->GetDocumentationExcerptName());
}

TSharedPtr<SWidget> SHeartGraphNodeBase::CreateAddInputButton()
{
	if (HeartEdGraphNode->CanUserAddInput())
	{
		TSharedPtr<SWidget> AddPinWidget;
		SAssignNew(AddPinWidget, SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding( 0, 0, 7, 0 )
		[
			SNew(SImage)
			.Image(FAppStyle::GetBrush(TEXT("Icons.PlusCircle")))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("HeartNodeAddPinButton", "Add pin"))
			.ColorAndOpacity(FLinearColor::White)
		];
	}

	return nullptr;
}

TSharedPtr<SWidget> SHeartGraphNodeBase::CreateAddOutputButton()
{
	if (HeartEdGraphNode->CanUserAddOutput())
	{
		TSharedPtr<SWidget> AddPinWidget;
		SAssignNew(AddPinWidget, SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("HeartNodeAddPinButton", "Add pin"))
			.ColorAndOpacity(FLinearColor::White)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(7, 0, 0, 0)
		[
			SNew(SImage)
			.Image(FAppStyle::GetBrush(TEXT("Icons.PlusCircle")))
		];
	}

	return nullptr;
}

FReply SHeartGraphNodeBase::OnAddHeartPin(const EEdGraphPinDirection Direction)
{
	switch (Direction)
	{
		case EGPD_Input:
			HeartEdGraphNode->AddUserInput();
			break;
		case EGPD_Output:
			HeartEdGraphNode->AddUserOutput();
			break;
		default: ;
	}

	return FReply::Handled();
}

void SHeartGraphNodeBase::CreateCommentBubble()
{
	if (GraphNode && GraphNode->SupportsCommentBubble())
	{
		// Create comment bubble
		TSharedPtr<SCommentBubble> CommentBubble;
		const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;

		SAssignNew(CommentBubble, SCommentBubble)
			.GraphNode(GraphNode)
			.Text(this, &SGraphNode::GetNodeComment)
			.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
			.OnToggled(this, &SGraphNode::OnCommentBubbleToggled)
			.ColorAndOpacity(CommentColor)
			.AllowPinning(true)
			.EnableTitleBarBubble(true)
			.EnableBubbleCtrls(true)
			.GraphLOD(this, &SGraphNode::GetCurrentLOD)
			.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

		GetOrAddSlot(ENodeZone::TopCenter)
			.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
			.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
			.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
			.VAlign(VAlign_Top)
			[
				CommentBubble.ToSharedRef()
			];
	}
}

#undef LOCTEXT_NAMESPACE