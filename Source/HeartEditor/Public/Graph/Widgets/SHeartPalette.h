// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "SGraphPalette.h"

class FHeartGraphAssetEditor;

/** Widget displaying a single item  */
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

	void Construct(const FArguments& InArgs, TWeakPtr<FHeartGraphAssetEditor> InHeartGraphAssetEditor);
	virtual ~SHeartPalette() override;

protected:
	void Refresh();
	void UpdateCategoryNames();

	// SGraphPalette
	virtual TSharedRef<SWidget> OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData) override;
	virtual void CollectAllActions(FGraphActionListBuilderBase& OutAllActions) override;
	// --

	FString GetFilterCategoryName() const;
	void CategorySelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);

	void OnActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& InActions, ESelectInfo::Type InSelectionType) const;

public:
	void ClearGraphActionMenuSelection() const;

protected:
	TWeakPtr<FHeartGraphAssetEditor> HeartGraphAssetEditorPtr;
	TArray<TSharedPtr<FString>> CategoryNames;
	TSharedPtr<STextComboBox> CategoryComboBox;
};
