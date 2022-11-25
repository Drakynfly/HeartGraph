// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Widgets/Navigation/SBreadcrumbTrail.h"

#include "Model/HeartGraph.h"

class FHeartGraphAssetEditor;
class UToolMenu;

//////////////////////////////////////////////////////////////////////////
// Heart Asset Breadcrumb

struct HEARTEDITOR_API FHeartBreadcrumb
{
	FString AssetPathName;
	FName InstanceName;

	FHeartBreadcrumb()
		: AssetPathName(FString())
		, InstanceName(NAME_None)
	{}

	FHeartBreadcrumb(const UHeartGraph* HeartGraph)
		: AssetPathName(HeartGraph->GetPathName())
		, InstanceName(HeartGraph->GetFName()) // @todo could be GetDisplayName or something
	{}
};

class HEARTEDITOR_API SHeartGraphAssetBreadcrumb final : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHeartGraphAssetBreadcrumb) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TWeakObjectPtr<UHeartGraph> InTemplateAsset);

private:
	void OnCrumbClicked(const FHeartBreadcrumb& Item) const;
	FText GetBreadcrumbText(const TWeakObjectPtr<UHeartGraph> HeartGraph) const;

	TWeakObjectPtr<UHeartGraph> TemplateAsset;
	TSharedPtr<SBreadcrumbTrail<FHeartBreadcrumb>> BreadcrumbTrail;
};

//////////////////////////////////////////////////////////////////////////
// Heart Graph Asset Toolbar

class HEARTEDITOR_API FHeartGraphAssetToolbar final : public TSharedFromThis<FHeartGraphAssetToolbar>
{
public:
	explicit FHeartGraphAssetToolbar(const TSharedPtr<FHeartGraphAssetEditor> InAssetEditor, UToolMenu* ToolbarMenu);

private:
	void BuildAssetToolbar(UToolMenu* ToolbarMenu) const;
	TSharedRef<SWidget> MakeDiffMenu() const;

	void BuildDebuggerToolbar(UToolMenu* ToolbarMenu);

private:
	TWeakPtr<FHeartGraphAssetEditor> HeartGraphAssetEditor;

	TSharedPtr<SHeartGraphAssetBreadcrumb> Breadcrumb;
};
