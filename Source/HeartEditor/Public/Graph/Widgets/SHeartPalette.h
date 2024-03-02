// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "SGraphPalette.h"

namespace Heart::AssetEditor
{
	class FHeartGraphEditor;
}

/** Widget displaying a single item  */
class HEARTEDITOR_API SHeartPaletteItem : public SGraphPaletteItem
{
public:
	SLATE_BEGIN_ARGS(SHeartPaletteItem) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData);
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;

private:
	TSharedRef<SWidget> CreateHotkeyDisplayWidget(const FSlateFontInfo& NameFont, const TSharedPtr<const FInputChord> HotkeyChord) const;
	virtual FText GetItemTooltip() const override;
};

/** Heart Palette  */
class HEARTEDITOR_API SHeartPalette : public SGraphPalette
{
public:
	SLATE_BEGIN_ARGS(SHeartPalette) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TWeakPtr<Heart::AssetEditor::FHeartGraphEditor> InHeartGraphAssetEditor);
	virtual ~SHeartPalette() override;

protected:
	// SGraphPalette
	virtual TSharedRef<SWidget> OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData) override;
	virtual void CollectAllActions(FGraphActionListBuilderBase& OutAllActions) override;
	// SGraphPalette

	void Refresh();
	void UpdateCategoryNames();

	FString GetFilterCategoryName() const;
	void CategorySelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);

	void OnActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& InActions, ESelectInfo::Type InSelectionType) const;

public:
	void ClearGraphActionMenuSelection() const;

protected:
	TWeakPtr<Heart::AssetEditor::FHeartGraphEditor> HeartGraphAssetEditorPtr;
	TArray<TSharedPtr<FString>> CategoryNames;
	TSharedPtr<STextComboBox> CategoryComboBox;
};