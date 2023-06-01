// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/Widgets/Nodes/SHeartGraphNode_Horizontal.h"

#include "GraphEditorSettings.h"
#include "HeartEditorStyle.h"
#include "IDocumentation.h"
#include "SLevelOfDetailBranchNode.h"
#include "TutorialMetaData.h"

#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "SHeartGraphNode_Horizontal"

void SHeartGraphNode_Horizontal::Construct(const FArguments& InArgs, UHeartEdGraphNode* InNode)
{
	SHeartGraphNodeBase::Construct(SHeartGraphNodeBase::FArguments(), InNode);
}

TSharedRef<SWidget> SHeartGraphNode_Horizontal::CreateNodeContentArea()
{
	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("NoBorder"))
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.FillWidth(1.0f)
			[
				SAssignNew(LeftNodeBox, SVerticalBox)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			[
				SAssignNew(RightNodeBox, SVerticalBox)
			]
		];
}

void SHeartGraphNode_Horizontal::AddPinButton(const TSharedPtr<SVerticalBox> OutputBox, const TSharedRef<SWidget> ButtonContent, const EEdGraphPinDirection Direction, const FString DocumentationExcerpt, const TSharedPtr<SToolTip> CustomTooltip)
{
	const FText PinTooltipText = (Direction == EGPD_Input) ?
		LOCTEXT("HeartNodeAddPinButton_InputTooltip", "Adds an input pin") :
		LOCTEXT("HeartNodeAddPinButton_OutputTooltip", "Adds an output pin");

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
		.OnClicked(this, &SHeartGraphNode_Horizontal::OnAddHeartPin, Direction)
		.IsEnabled(this, &SHeartGraphNode_Horizontal::IsNodeEditable)
		.ToolTipText(PinTooltipText)
		.ToolTip(Tooltip)
		.Visibility(this, &SHeartGraphNode_Horizontal::IsAddPinButtonVisible)
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

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SHeartGraphNode_Horizontal::UpdateGraphNode()
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
							.ColorAndOpacity(this, &SHeartGraphNode_Horizontal::GetNodeTitleIconColor)
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
			.UseLowDetailSlot(this, &SHeartGraphNode_Horizontal::UseLowDetailNodeTitles)
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

	ContentScale.Bind(this, &SGraphNode::GetContentScale);

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

	GetOrAddSlot(ENodeZone::Center)
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
							.ColorAndOpacity(this, &SHeartGraphNodeBase::GetNodeBodyColor)
						]
					+ SOverlay::Slot()
						[
							InnerVerticalBox.ToSharedRef()
						]
			]
		];

	CreateCommentBubble();
	CreateBelowWidgetControls(MainVerticalBox);
	CreatePinWidgets();

	if (auto&& Button = CreateAddInputButton())
	{
		AddPinButton(RightNodeBox, Button.ToSharedRef(), EGPD_Input);
	}

	if (auto&& Button = CreateAddOutputButton())
	{
		AddPinButton(LeftNodeBox, Button.ToSharedRef(), EGPD_Output);
	}

	CreateBelowPinControls(InnerVerticalBox);
	CreateAdvancedViewArrow(InnerVerticalBox);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE