// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartGraphSchemaCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "HeartEditorModule.h"
#include "ModelView/HeartGraphSchema.h"

#define LOCTEXT_NAMESPACE "HeartGraphSchemaCustomization"

TSharedRef<IDetailCustomization> FHeartGraphSchemaCustomization::MakeInstance()
{
	return MakeShared<FHeartGraphSchemaCustomization>();
}

FHeartGraphSchemaCustomization::FHeartGraphSchemaCustomization()
{
	const FHeartEditorModule& HeartEditorModule = FModuleManager::LoadModuleChecked<FHeartEditorModule>("HeartEditor");

	StyleOptions.Append(HeartEditorModule.GetSlateStyles());
}

void FHeartGraphSchemaCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	StyleProp = DetailBuilder.GetProperty(UHeartGraphSchema::DefaultEditorStylePropertyName());

	if (StyleProp.IsValid())
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
				.OnSelectionChanged(this, &FHeartGraphSchemaCustomization::OnStyleSelectionChanged)
				.OnGenerateWidget(this, &FHeartGraphSchemaCustomization::OnGenerateStyleWidget)
				[
					SNew(STextBlock)
						.Text(this, &FHeartGraphSchemaCustomization::GetSelectedStyle)
				]
			];
	}
}

void FHeartGraphSchemaCustomization::OnStyleSelectionChanged(const FName Name, ESelectInfo::Type SelectInfo)
{
	if (StyleProp.IsValid())
	{
		StyleProp->SetValue(Name);
	}
}

TSharedRef<SWidget> FHeartGraphSchemaCustomization::OnGenerateStyleWidget(const FName Style)
{
	return SNew(STextBlock)
		.Text(FText::FromString(FName::NameToDisplayString(Style.ToString(), false)));
}

FText FHeartGraphSchemaCustomization::GetSelectedStyle() const
{
	FName Value;
	if (StyleProp.IsValid())
	{
		StyleProp->GetValue(Value);
	}
	return FText::FromString(FName::NameToDisplayString(Value.ToString(), false));
}

#undef LOCTEXT_NAMESPACE