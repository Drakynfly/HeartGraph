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
	const FHeartEditorModule& HeartEditorModule = FModuleManager::GetModuleChecked<FHeartEditorModule>("HeartEditor");

	StyleOptions.Append(HeartEditorModule.GetSlateStyles());
	PolicyOptions.Append(HeartEditorModule.GetDrawingPolicies());
}

void FHeartGraphSchemaCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	SlateStyleProp = DetailBuilder.GetProperty(UHeartGraphSchema::DefaultEditorStylePropertyName());
	DrawingPolicyProp = DetailBuilder.GetProperty(UHeartGraphSchema::ConnectionDrawingPolicyPropertyName());

	IDetailCategoryBuilder& EditorCategory = DetailBuilder.EditCategory("Editor");

	if (SlateStyleProp.IsValid())
	{
		DetailBuilder.HideProperty(SlateStyleProp);

		FDetailWidgetRow& EditorSlateStyleRow = EditorCategory.AddCustomRow(LOCTEXT("EditorSlateStyleSearchString", "Editor Slate Style"));

		EditorSlateStyleRow.NameContent()
			[
				SlateStyleProp->CreatePropertyNameWidget()
			];

		EditorSlateStyleRow.ValueContent()
			[
				SNew(SComboBox<FName>)
				.OptionsSource(&StyleOptions)
				.OnSelectionChanged(this, &FHeartGraphSchemaCustomization::OnStyleSelectionChanged)
				.OnGenerateWidget(this, &FHeartGraphSchemaCustomization::OnGenerateNameWidget)
				[
					SNew(STextBlock)
						.Text(this, &FHeartGraphSchemaCustomization::GetSelectedStyle)
						.Font(IPropertyTypeCustomizationUtils::GetRegularFont())
				]
			];
	}

	if (DrawingPolicyProp.IsValid())
	{
		DetailBuilder.HideProperty(DrawingPolicyProp);

		FDetailWidgetRow& DrawingPolicyRow = EditorCategory.AddCustomRow(LOCTEXT("ConnectionDrawingPolicySearchString", "Connection Drawing Policy"));

		DrawingPolicyRow.NameContent()
			[
				DrawingPolicyProp->CreatePropertyNameWidget()
			];

		DrawingPolicyRow.ValueContent()
			[
				SNew(SComboBox<FName>)
				.OptionsSource(&PolicyOptions)
				.OnSelectionChanged(this, &FHeartGraphSchemaCustomization::OnPolicySelectionChanged)
				.OnGenerateWidget(this, &FHeartGraphSchemaCustomization::OnGenerateNameWidget)
				[
					SNew(STextBlock)
						.Text(this, &FHeartGraphSchemaCustomization::GetSelectedPolicy)
						.Font(IPropertyTypeCustomizationUtils::GetRegularFont())
				]
			];
	}
}

void FHeartGraphSchemaCustomization::OnStyleSelectionChanged(const FName Name, ESelectInfo::Type SelectInfo)
{
	if (SlateStyleProp.IsValid())
	{
		SlateStyleProp->SetValue(Name);
	}
}

FText FHeartGraphSchemaCustomization::GetSelectedStyle() const
{
	FName Value;
	if (SlateStyleProp.IsValid())
	{
		SlateStyleProp->GetValue(Value);
	}
	return NameToText(Value);
}

void FHeartGraphSchemaCustomization::OnPolicySelectionChanged(const FName Name, ESelectInfo::Type SelectInfo)
{
	if (DrawingPolicyProp.IsValid())
	{
		DrawingPolicyProp->SetValue(Name);
	}
}

FText FHeartGraphSchemaCustomization::GetSelectedPolicy() const
{
	FName Value;
	if (DrawingPolicyProp.IsValid())
	{
		DrawingPolicyProp->GetValue(Value);
	}
	return NameToText(Value);
}

FText FHeartGraphSchemaCustomization::NameToText(const FName Name)
{
	return FText::FromString(FName::NameToDisplayString(Name.ToString(), false));
}

TSharedRef<SWidget> FHeartGraphSchemaCustomization::OnGenerateNameWidget(const FName Style)
{
	return SNew(STextBlock)
		.Text(NameToText(Style))
		.Font(IPropertyTypeCustomizationUtils::GetRegularFont());
}

#undef LOCTEXT_NAMESPACE
