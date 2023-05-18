// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/Widgets/Nodes/SHeartGraphNode_Vertical.h"

#include "GraphEditorSettings.h"
#include "HeartEditorStyle.h"
#include "IDocumentation.h"
#include "SGraphPin.h"
#include "SLevelOfDetailBranchNode.h"
#include "TutorialMetaData.h"

#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "SHeartGraphNode_Vertical"

void SHeartGraphNode_Vertical::Construct(const FArguments& InArgs, UHeartEdGraphNode* InNode)
{
	SHeartGraphNodeBase::Construct(SHeartGraphNodeBase::FArguments(), InNode);
}

TSharedRef<SWidget> SHeartGraphNode_Vertical::CreateNodeContentArea()
{
	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("NoBorder"))
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Top)
			.FillHeight(1.0f)
			[
				SAssignNew(TopNodeBox, SHorizontalBox)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Center)
			.FillHeight(1.0f)
			.Padding(Settings->GetNonPinNodeBodyPadding())
			[
				SAssignNew(CenterNodeBox, SHorizontalBox)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.FillHeight(1.0f)
			.VAlign(VAlign_Bottom)
			[
				SAssignNew(BottomNodeBox, SHorizontalBox)
			]
		];}

void SHeartGraphNode_Vertical::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = (PinObj != nullptr) && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}

	if (PinToAdd->GetDirection() == EGPD_Input)
	{
		TopNodeBox->AddSlot()
			.AutoWidth()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Top)
			.Padding(Settings->GetInputPinPadding())
			[
				PinToAdd
			];
		InputPins.Add(PinToAdd);
	}
	else // Direction == EGPD_Output
	{
		BottomNodeBox->AddSlot()
			.AutoWidth()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Bottom)
			.Padding(Settings->GetOutputPinPadding())
			[
				PinToAdd
			];
		OutputPins.Add(PinToAdd);
	}
}

void SHeartGraphNode_Vertical::AddPinButton(const TSharedPtr<SHorizontalBox> OutputBox, const TSharedRef<SWidget> ButtonContent,
											const EEdGraphPinDirection Direction, const FString DocumentationExcerpt, const TSharedPtr<SToolTip> CustomTooltip)
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
		.OnClicked(this, &SHeartGraphNode_Vertical::OnAddHeartPin, Direction)
		.IsEnabled(this, &SHeartGraphNode_Vertical::IsNodeEditable)
		.ToolTipText(PinTooltipText)
		.ToolTip(Tooltip)
		.Visibility(this, &SHeartGraphNode_Vertical::IsAddPinButtonVisible)
		[
			ButtonContent
		];

	AddPinButton->SetCursor(EMouseCursor::Hand);

	FMargin AddPinPadding = (Direction == EGPD_Input) ? Settings->GetInputPinPadding() : Settings->GetOutputPinPadding();
	AddPinPadding.Top += 6.0f;

	OutputBox->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(AddPinPadding)
		[
			AddPinButton
		];
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SHeartGraphNode_Vertical::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	TopNodeBox.Reset();
	CenterNodeBox.Reset();
	BottomNodeBox.Reset();

	//	     ______________________
	//		|	  (^) (^) (^)	   |
	//		|	   T   O   P	   |
	//		+-------+------+-------+
	//	    |      TITLE AREA      |
	//		+-------+------+-------+
	//		|   B  O  T  T  O  M   |
	//		|  (^)(^)(^)(^)(^)(^)  |
	//	     ______________________
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
							.ColorAndOpacity(this, &SHeartGraphNode_Vertical::GetNodeTitleIconColor)
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
			.UseLowDetailSlot(this, &SHeartGraphNode_Vertical::UseLowDetailNodeTitles)
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
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				CreateNodeContentArea()
			];

	CenterNodeBox->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		.Padding(Settings->GetNonPinNodeBodyPadding())
		[
			TitleAreaWidget
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
		AddPinButton(TopNodeBox, Button.ToSharedRef(), EGPD_Input);
	}

	if (auto&& Button = CreateAddOutputButton())
	{
		AddPinButton(BottomNodeBox, Button.ToSharedRef(), EGPD_Output);
	}

	CreateBelowPinControls(InnerVerticalBox);
	CreateAdvancedViewArrow(InnerVerticalBox);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE