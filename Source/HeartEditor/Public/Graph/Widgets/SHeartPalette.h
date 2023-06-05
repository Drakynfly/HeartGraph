// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "SGraphPalette.h"

namespace Heart::AssetEditor
{
	class FAssetEditor;
}

/** Widget displaying a single item  */
// @todo, double-clicking on this should open up the node BP
class HEARTEDITOR_API SHeartPaletteItem : public SGraphPaletteItem
{
public:
	SLATE_BEGIN_ARGS(SHeartPaletteItem) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData);

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

	void Construct(const FArguments& InArgs, TWeakPtr<Heart::AssetEditor::FAssetEditor> InHeartGraphAssetEditor);
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
	TWeakPtr<Heart::AssetEditor::FAssetEditor> HeartGraphAssetEditorPtr;
	TArray<TSharedPtr<FString>> CategoryNames;
	TSharedPtr<STextComboBox> CategoryComboBox;
};
