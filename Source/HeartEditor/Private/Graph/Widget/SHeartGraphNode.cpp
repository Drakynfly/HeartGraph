// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/Widgets/SHeartGraphNode.h"
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
#include "SGraphPin.h"
#include "SlateOptMacros.h"
#include "SLevelOfDetailBranchNode.h"
#include "SNodePanel.h"
#include "Styling/SlateColor.h"
#include "TutorialMetaData.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "SHeartGraphNode"

SHeartGraphPin::SHeartGraphPin()
{
	//PinColorModifier =
}

void SHeartGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InPin);
	bUsePinColorForText = true;
}

void SHeartGraphNode::Construct(const FArguments& InArgs, UHeartEdGraphNode* InNode)
{
	GraphNode = InNode;
	HeartEdGraphNode = InNode;

	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

void SHeartGraphNode::GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const
{
	//const FString Description = HeartEdGraphNode->GetNodeDescription();
	//if (!Description.IsEmpty())
	{
		//const FGraphInformationPopupInfo DescriptionPopup = FGraphInformationPopupInfo(nullptr, BackgroundColor, Description);
		//Popups.Add(DescriptionPopup);
	}
}

const FSlateBrush* SHeartGraphNode::GetShadowBrush(bool bSelected) const
{
	return SGraphNode::GetShadowBrush(bSelected);
}

void SHeartGraphNode::GetOverlayBrushes(bool bSelected, const FVector2D WidgetSize, TArray<FOverlayBrushInfo>& Brushes) const
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

void SHeartGraphNode::GetPinBrush(const bool bLeftSide, const float WidgetWidth, const int32 PinIndex, const FHeartBreakpoint& Breakpoint, TArray<FOverlayBrushInfo>& Brushes) const
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

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SHeartGraphNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	//	     ______________________
	//	    |      TITLE AREA      |
	//	    +-------+------+-------+
	//	    | (>) L |      | R (>) |
	//	    | (>) E |      | I (>) |
	//	    | (>) F |      | G (>) |
	//	    | (>) T |      | H (>) |
	//	    |       |      | T (>) |
	//	    |_______|______|_______|
	//
	TSharedPtr<SVerticalBox> MainVerticalBox;
	SetupErrorReporting();

	const TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	// Get node icon
	IconColor = FLinearColor::White;
	const FSlateBrush* IconBrush = nullptr;
	if (GraphNode && GraphNode->ShowPaletteIconOnNode())
	{
		IconBrush = GraphNode->GetIconAndTint(IconColor).GetOptionalIcon();
	}

	const TSharedRef<SOverlay> DefaultTitleAreaWidget = SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			[
				SNew(SBorder)
				.BorderImage(FHeartEditorStyle::GetBrush("Heart.Node.Title"))
				// The extra margin on the right is for making the color spill stretch well past the node title
				.Padding(FMargin(10, 5, 30, 3))
				.BorderBackgroundColor(this, &SGraphNode::GetNodeTitleColor)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
						.VAlign(VAlign_Top)
						.Padding(FMargin(0.f, 0.f, 4.f, 0.f))
						.AutoWidth()
						[
							SNew(SImage)
							.Image(IconBrush)
							.ColorAndOpacity(this, &SHeartGraphNode::GetNodeTitleIconColor)
						]
					+ SHorizontalBox::Slot()
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
								.AutoHeight()
								[
									CreateTitleWidget(NodeTitle)
								]
							+ SVerticalBox::Slot()
								.AutoHeight()
								[
									NodeTitle.ToSharedRef()
								]
						]
				]
			]
		];

	SetDefaultTitleAreaWidget(DefaultTitleAreaWidget);

	const TSharedRef<SWidget> TitleAreaWidget =
		SNew(SLevelOfDetailBranchNode)
			.UseLowDetailSlot(this, &SHeartGraphNode::UseLowDetailNodeTitles)
			.LowDetail()
			[
				SNew(SBorder)
					.BorderImage(FHeartEditorStyle::GetBrush("Heart.Node.Title"))
					.Padding(FMargin(75.0f, 22.0f)) // Saving enough space for a 'typical' title so the transition isn't quite so abrupt
					.BorderBackgroundColor(this, &SGraphNode::GetNodeTitleColor)
			]
			.HighDetail()
			[
				DefaultTitleAreaWidget
			];

	// Setup a meta tag for this node
	FGraphNodeMetaData TagMeta(TEXT("HeartGraphNode"));
	PopulateMetaTag(&TagMeta);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);

	const TSharedPtr<SVerticalBox> InnerVerticalBox = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			.Padding(Settings->GetNonPinNodeBodyPadding())
			[
				TitleAreaWidget
			]
		+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				CreateNodeContentArea()
			];

	const TSharedPtr<SWidget> EnabledStateWidget = GetEnabledStateWidget();
	if (EnabledStateWidget.IsValid())
	{
		InnerVerticalBox->AddSlot()
			.AutoHeight()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			.Padding(FMargin(2, 0))
			[
				EnabledStateWidget.ToSharedRef()
			];
	}

	InnerVerticalBox->AddSlot()
		.AutoHeight()
		.Padding(Settings->GetNonPinNodeBodyPadding())
		[
			ErrorReporting->AsWidget()
		];

	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(MainVerticalBox, SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SOverlay)
					.AddMetaData<FGraphNodeMetaData>(TagMeta)
					+ SOverlay::Slot()
						.Padding(Settings->GetNonPinNodeBodyPadding())
						[
							SNew(SImage)
							.Image(GetNodeBodyBrush())
							.ColorAndOpacity(this, &SHeartGraphNode::GetNodeBodyColor)
						]
					+ SOverlay::Slot()
						[
							InnerVerticalBox.ToSharedRef()
						]
			]
		];

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

	CreateBelowWidgetControls(MainVerticalBox);
	CreatePinWidgets();
	CreateInputSideAddButton(LeftNodeBox);
	CreateOutputSideAddButton(RightNodeBox);
	CreateBelowPinControls(InnerVerticalBox);
	CreateAdvancedViewArrow(InnerVerticalBox);
}

void SHeartGraphNode::UpdateErrorInfo()
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

TSharedRef<SWidget> SHeartGraphNode::CreateTitleWidget(TSharedPtr<SNodeTitle> NodeTitle)
{
	SAssignNew(InlineEditableText, SInlineEditableTextBlock)
		.Style(FAppStyle::Get(), "Graph.Node.NodeTitleInlineEditableText")
		.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
		.OnVerifyTextChanged(this, &SHeartGraphNode::OnVerifyNameTextChanged)
		.OnTextCommitted(this, &SHeartGraphNode::OnNameTextCommited)
		.IsReadOnly(this, &SHeartGraphNode::IsNameReadOnly)
		.IsSelected(this, &SHeartGraphNode::IsSelectedExclusively);
		InlineEditableText->SetColorAndOpacity(TAttribute<FLinearColor>::Create(TAttribute<FLinearColor>::FGetter::CreateSP(this, &SHeartGraphNode::GetNodeTitleTextColor)));

	return InlineEditableText.ToSharedRef();
}

TSharedRef<SWidget> SHeartGraphNode::CreateNodeContentArea()
{
	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("NoBorder"))
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.FillWidth(1.0f)
			[
				SAssignNew(LeftNodeBox, SVerticalBox)
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			[
				SAssignNew(RightNodeBox, SVerticalBox)
			]
		];
}

const FSlateBrush* SHeartGraphNode::GetNodeBodyBrush() const
{
	return FHeartEditorStyle::GetBrush("Heart.Node.Body");
}

FSlateColor SHeartGraphNode::GetNodeTitleColor() const
{
	FLinearColor ReturnTitleColor = GraphNode->IsDeprecated() ? FLinearColor::Red : GetNodeObj()->GetNodeTitleColor();
	ReturnTitleColor.A = FadeCurve.GetLerp();
	return ReturnTitleColor;
}

FSlateColor SHeartGraphNode::GetNodeBodyColor() const
{
	const FLinearColor ReturnBodyColor = GraphNode->GetNodeBodyTintColor();
	return ReturnBodyColor;
}

FSlateColor SHeartGraphNode::GetNodeTitleIconColor() const
{
	const FLinearColor ReturnIconColor = IconColor;
	return ReturnIconColor;
}

FLinearColor SHeartGraphNode::GetNodeTitleTextColor() const
{
	const FLinearColor ReturnTextColor = FLinearColor::White;
	return ReturnTextColor;
}

void SHeartGraphNode::CreateStandardPinWidget(UEdGraphPin* Pin)
{
	const TSharedPtr<SGraphPin> NewPin = SNew(SHeartGraphPin, Pin);

	if (HeartEdGraphNode->GetHeartGraphNode())
	{
		if (Pin->Direction == EGPD_Input)
		{
			//if (HeartEdGraphNode->GetHeartGraphNode()->GetInputPins().Num() == 1 && Pin->PinName == UHeartGraphNode::DefaultInputPin.PinName)
			{
			//	NewPin->SetShowLabel(false);
			}
		}
		else
		{
			//if (HeartEdGraphNode->GetHeartGraphNode()->GetOutputPins().Num() == 1 && Pin->PinName == UHeartGraphNode::DefaultOutputPin.PinName)
			{
			//	NewPin->SetShowLabel(false);
			}
		}
	}

	this->AddPin(NewPin.ToSharedRef());
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedPtr<SToolTip> SHeartGraphNode::GetComplexTooltip()
{
	return IDocumentation::Get()->CreateToolTip(TAttribute<FText>(this, &SGraphNode::GetNodeTooltip), nullptr, GraphNode->GetDocumentationLink(), GraphNode->GetDocumentationExcerptName());
}

void SHeartGraphNode::CreateInputSideAddButton(TSharedPtr<SVerticalBox> OutputBox)
{
	if (HeartEdGraphNode->CanUserAddInput())
	{
		TSharedPtr<SWidget> AddPinWidget;
		SAssignNew(AddPinWidget, SHorizontalBox)
		+SHorizontalBox::Slot()
		.AutoWidth()
		. VAlign(VAlign_Center)
		. Padding( 0,0,7,0 )
		[
			SNew(SImage)
			.Image(FAppStyle::GetBrush(TEXT("Icons.PlusCircle")))
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("HeartNodeAddPinButton", "Add pin"))
			.ColorAndOpacity(FLinearColor::White)
		];

		AddPinButton(OutputBox, AddPinWidget.ToSharedRef(), EGPD_Input);
	}
}

void SHeartGraphNode::CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox)
{
	if (HeartEdGraphNode->CanUserAddOutput())
	{
		TSharedPtr<SWidget> AddPinWidget;
		SAssignNew(AddPinWidget, SHorizontalBox)
		+SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("HeartNodeAddPinButton", "Add pin"))
			.ColorAndOpacity(FLinearColor::White)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(7,0,0,0)
		[
			SNew(SImage)
			.Image(FAppStyle::GetBrush(TEXT("Icons.PlusCircle")))
		];

		AddPinButton(OutputBox, AddPinWidget.ToSharedRef(), EGPD_Output);
	}
}

void SHeartGraphNode::AddPinButton(TSharedPtr<SVerticalBox> OutputBox, const TSharedRef<SWidget> ButtonContent, const EEdGraphPinDirection Direction, const FString DocumentationExcerpt, const TSharedPtr<SToolTip> CustomTooltip)
{
	const FText PinTooltipText = (Direction == EGPD_Input) ? LOCTEXT("HeartNodeAddPinButton_InputTooltip", "Adds an input pin") : LOCTEXT("HeartNodeAddPinButton_OutputTooltip", "Adds an output pin");
	TSharedPtr<SToolTip> Tooltip;

	if (CustomTooltip.IsValid())
	{
		Tooltip = CustomTooltip;
	}
	else if (!DocumentationExcerpt.IsEmpty())
	{
		Tooltip = IDocumentation::Get()->CreateToolTip(PinTooltipText, nullptr, GraphNode->GetDocumentationLink(), DocumentationExcerpt);
	}

	const TSharedRef<SButton> AddPinButton = SNew(SButton)
	.ContentPadding(0.0f)
	.ButtonStyle(FAppStyle::Get(), "NoBorder")
	.OnClicked(this, &SHeartGraphNode::OnAddHeartPin, Direction)
	.IsEnabled(this, &SHeartGraphNode::IsNodeEditable)
	.ToolTipText(PinTooltipText)
	.ToolTip(Tooltip)
	.Visibility(this, &SHeartGraphNode::IsAddPinButtonVisible)
	[
		ButtonContent
	];

	AddPinButton->SetCursor(EMouseCursor::Hand);

	FMargin AddPinPadding = (Direction == EGPD_Input) ? Settings->GetInputPinPadding() : Settings->GetOutputPinPadding();
	AddPinPadding.Top += 6.0f;

	OutputBox->AddSlot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(AddPinPadding)
		[
			AddPinButton
		];
}

FReply SHeartGraphNode::OnAddHeartPin(const EEdGraphPinDirection Direction)
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

#undef LOCTEXT_NAMESPACE
