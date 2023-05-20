// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/Widgets/SHeartPalette.h"
#include "Graph/HeartGraphAssetEditor.h"
#include "HeartEditorCommands.h"
#include "HeartRegistryEditorSubsystem.h"
#include "Graph/HeartEdGraphSchema.h"
#include "Graph/HeartEdGraphSchema_Actions.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

#include "Fonts/SlateFontInfo.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateColor.h"
#include "Widgets/Input/STextComboBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "HeartGraphPalette"

void SHeartPaletteItem::Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData)
{
	const FSlateFontInfo NameFont = FCoreStyle::GetDefaultFontStyle("Regular", 10);

	check(InCreateData->Action.IsValid());

	const TSharedPtr<FEdGraphSchemaAction> GraphAction = InCreateData->Action;
	ActionPtr = InCreateData->Action;

	// Get the hotkey chord if one exists for this action
	TSharedPtr<const FInputChord> HotkeyChord;

	if (FHeartSpawnNodeCommands::IsRegistered())
	{
		if (GraphAction->GetTypeId() == FHeartGraphSchemaAction_NewNode::StaticGetTypeId())
		{
			const UClass* HeartGraphNodeClass = StaticCastSharedPtr<FHeartGraphSchemaAction_NewNode>(GraphAction)->GetNodeClass();
			HotkeyChord = FHeartSpawnNodeCommands::Get().GetChordByClass(HeartGraphNodeClass);
		}
		else if (GraphAction->GetTypeId() == FHeartGraphSchemaAction_NewComment::StaticGetTypeId())
		{
			HotkeyChord = FHeartSpawnNodeCommands::Get().GetChordByClass(UHeartGraphNode::StaticClass());
		}
	}

	// Find icons
	const FSlateBrush* IconBrush = FAppStyle::GetBrush(TEXT("NoBrush"));
	const FSlateColor IconColor = FSlateColor::UseForeground();
	const FText IconToolTip = GraphAction->GetTooltipDescription();
	constexpr bool bIsReadOnly = false;

	const TSharedRef<SWidget> IconWidget = CreateIconWidget(IconToolTip, IconBrush, IconColor);
	const TSharedRef<SWidget> NameSlotWidget = CreateTextSlotWidget(InCreateData, bIsReadOnly);
	const TSharedRef<SWidget> HotkeyDisplayWidget = CreateHotkeyDisplayWidget(NameFont, HotkeyChord);

	// Create the actual widget
	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				IconWidget
			]
		+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			.VAlign(VAlign_Center)
			.Padding(3, 0)
			[
				NameSlotWidget
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			[
				HotkeyDisplayWidget
			]
	];
}

TSharedRef<SWidget> SHeartPaletteItem::CreateHotkeyDisplayWidget(const FSlateFontInfo& NameFont, const TSharedPtr<const FInputChord> HotkeyChord) const
{
	FText HotkeyText;
	if (HotkeyChord.IsValid())
	{
		HotkeyText = HotkeyChord->GetInputText();
	}

	return SNew(STextBlock)
		.Text(HotkeyText)
		.Font(NameFont);
}

FText SHeartPaletteItem::GetItemTooltip() const
{
	return ActionPtr.Pin()->GetTooltipDescription();
}

void SHeartPalette::Construct(const FArguments& InArgs, TWeakPtr<FHeartGraphAssetEditor> InHeartGraphAssetEditor)
{
	HeartGraphAssetEditorPtr = InHeartGraphAssetEditor;

	UpdateCategoryNames();

	if (auto&& EditorRegistry = GEditor->GetEditorSubsystem<UHeartRegistryEditorSubsystem>())
	{
		EditorRegistry->OnRefreshPalettes.AddSP(this, &SHeartPalette::Refresh);
	}

	UEdGraph* EdGraph = nullptr;
	if (auto&& HeartGraph = InHeartGraphAssetEditor.Pin()->GetHeartGraph())
	{
		EdGraph = HeartGraph->GetEdGraph();
	}

	ChildSlot
	[
		SNew(SBorder)
			.Padding(2.0f)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot() // Filter UI
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.VAlign(VAlign_Center)
							[
								SAssignNew(CategoryComboBox, STextComboBox)
									.OptionsSource(&CategoryNames)
									.OnSelectionChanged(this, &SHeartPalette::CategorySelectionChanged)
									.InitiallySelectedItem(CategoryNames[0])
							]
					]
				+ SVerticalBox::Slot() // Content list
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SAssignNew(GraphActionMenu, SGraphActionMenu)
							.OnActionDragged(this, &SHeartPalette::OnActionDragged)
							.OnActionSelected(this, &SHeartPalette::OnActionSelected)
							.OnCreateWidgetForAction(this, &SHeartPalette::OnCreateWidgetForAction)
							.OnCollectAllActions(this, &SHeartPalette::CollectAllActions)
							.AutoExpandActionMenu(true)
							.GraphObj(EdGraph)
					]
			]
	];
}

SHeartPalette::~SHeartPalette()
{
	if (auto&& EditorRegistry = GEditor->GetEditorSubsystem<UHeartRegistryEditorSubsystem>())
	{
		EditorRegistry->OnRefreshPalettes.RemoveAll(this);
	}
}

TSharedRef<SWidget> SHeartPalette::OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData)
{
	return SNew(SHeartPaletteItem, InCreateData);
}

void SHeartPalette::CollectAllActions(FGraphActionListBuilderBase& OutAllActions)
{
	const UClass* AssetClass = UHeartGraph::StaticClass();

	auto&& HeartGraphAssetEditor = HeartGraphAssetEditorPtr.Pin();
	if (HeartGraphAssetEditor && HeartGraphAssetEditor->GetHeartGraph())
	{
		AssetClass = HeartGraphAssetEditor->GetHeartGraph()->GetClass();
	}

	FGraphActionMenuBuilder ActionMenuBuilder;
	UHeartEdGraphSchema::GetPaletteActions(ActionMenuBuilder, AssetClass, GetFilterCategoryName());
	OutAllActions.Append(ActionMenuBuilder);
}

void SHeartPalette::Refresh()
{
	const FString LastSelectedCategory = CategoryComboBox->GetSelectedItem().IsValid() ? *CategoryComboBox->GetSelectedItem().Get() : FString();

	UpdateCategoryNames();
	RefreshActionsList(true);

	// refresh list of category and currently selected category
	CategoryComboBox->RefreshOptions();
	TSharedPtr<FString> SelectedCategory = CategoryNames[0];
	if (!LastSelectedCategory.IsEmpty())
	{
		for (const TSharedPtr<FString>& CategoryName : CategoryNames)
		{
			if (*CategoryName.Get() == LastSelectedCategory)
			{
				SelectedCategory = CategoryName;
				break;
			}
		}
	}
	CategoryComboBox->SetSelectedItem(SelectedCategory);
}

void SHeartPalette::UpdateCategoryNames()
{
	CategoryNames = {MakeShareable(new FString(TEXT("All")))};

	auto&& HeartGraphAssetEditor = HeartGraphAssetEditorPtr.Pin();
	if (HeartGraphAssetEditor && HeartGraphAssetEditor->GetHeartGraph())
	{
		CategoryNames.Append(UHeartEdGraphSchema::GetHeartGraphNodeCategories(HeartGraphAssetEditor->GetHeartGraph()->GetClass()));
	}
}

FString SHeartPalette::GetFilterCategoryName() const
{
	if (CategoryComboBox.IsValid() && CategoryComboBox->GetSelectedItem() != CategoryNames[0])
	{
		return *CategoryComboBox->GetSelectedItem();
	}

	return FString();
}

void SHeartPalette::CategorySelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	RefreshActionsList(true);
}

void SHeartPalette::OnActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& InActions, ESelectInfo::Type InSelectionType) const
{
	if (InSelectionType == ESelectInfo::OnMouseClick || InSelectionType == ESelectInfo::OnKeyPress || InSelectionType == ESelectInfo::OnNavigation || InActions.Num() == 0)
	{
		auto&& HeartGraphAssetEditor = HeartGraphAssetEditorPtr.Pin();
		if (HeartGraphAssetEditor)
		{
			HeartGraphAssetEditor->SetUISelectionState(FHeartGraphAssetEditor::PaletteTab);
		}
	}
}

void SHeartPalette::ClearGraphActionMenuSelection() const
{
	GraphActionMenu->SelectItemByName(NAME_None);
}

#undef LOCTEXT_NAMESPACE
