// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartGraphNodeCustomization.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "HeartEditorModule.h"
#include "Graph/HeartEdGraphUtils.h"
#include "Model/HeartGraphNode.h"

#define LOCTEXT_NAMESPACE "HeartGraphNodeCustomization"

TSharedRef<IDetailCustomization> FHeartGraphNodeCustomization::MakeInstance()
{
	return MakeShared<FHeartGraphNodeCustomization>();
}

FHeartGraphNodeCustomization::FHeartGraphNodeCustomization()
{
	const FHeartEditorModule& HeartEditorModule = FModuleManager::GetModuleChecked<FHeartEditorModule>("HeartEditor");

	StyleOptions.Add(Heart::GraphUtils::DefaultStyle);
	StyleOptions.Append(HeartEditorModule.GetSlateStyles());
}

void FHeartGraphNodeCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);

	if (Objects.IsEmpty())
	{
		return;
	}

	const bool EditingAsset = Objects[0]->IsAsset();

	if (EditingAsset)
	{
		// @todo this is broken. GetProperty cannot find the sparse data property
		StyleProp = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(FHeartGraphNodeSparseClassData, EditorSlateStyle));

		if (StyleProp->IsValidHandle())
		{
			DetailBuilder.HideProperty(StyleProp);

			IDetailCategoryBuilder& EditorCategory = DetailBuilder.EditCategory("Editor");
			FDetailWidgetRow& EditorSlateStyleRow = EditorCategory.AddCustomRow(LOCTEXT("EditorSlateStyleSearchString", "Editor Slate Style"));

			EditorSlateStyleRow.NameContent()
				[
					StyleProp->CreatePropertyNameWidget()
				];

			EditorSlateStyleRow.ValueContent()
				[
					SNew(SComboBox<FName>)
					.OptionsSource(&StyleOptions)
					.OnSelectionChanged(this, &FHeartGraphNodeCustomization::OnStyleSelectionChanged)
					.OnGenerateWidget(this, &FHeartGraphNodeCustomization::OnGenerateStyleWidget)
					[
						SNew(STextBlock)
							.Text(this, &FHeartGraphNodeCustomization::GetSelectedStyle)
							.Font(IPropertyTypeCustomizationUtils::GetRegularFont())
					]
				];
		}
	}
}

void FHeartGraphNodeCustomization::OnStyleSelectionChanged(const FName Name, ESelectInfo::Type SelectInfo)
{
	if (StyleProp.IsValid())
	{
		StyleProp->SetValue(Name);
	}
}

TSharedRef<SWidget> FHeartGraphNodeCustomization::OnGenerateStyleWidget(const FName Style)
{
	FText Tooltip;

	if (Style == Heart::GraphUtils::DefaultStyle)
	{
		Tooltip = LOCTEXT("DefaultOptionTooltip", "Use the Default Editor Style of the graph's Schema");
	}

	return SNew(STextBlock)
		.Text(FText::FromString(FName::NameToDisplayString(Style.ToString(), false)))
		.Font(IPropertyTypeCustomizationUtils::GetRegularFont())
		.ToolTipText(Tooltip);
}

FText FHeartGraphNodeCustomization::GetSelectedStyle() const
{
	FName Value;
	if (StyleProp.IsValid())
	{
		StyleProp->GetValue(Value);
	}

	return FText::FromString(FName::NameToDisplayString(Value.ToString(), false));
}

#undef LOCTEXT_NAMESPACE