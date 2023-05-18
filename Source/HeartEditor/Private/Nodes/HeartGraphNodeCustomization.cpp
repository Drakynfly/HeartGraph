// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartGraphNodeCustomization.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "HeartEditorModule.h"
#include "IDetailChildrenBuilder.h"
#include "Model/HeartGraphNode.h"

#define LOCTEXT_NAMESPACE "HeartGraphNodeEditorDataTempCustomization"

TSharedRef<IPropertyTypeCustomization> FHeartGraphNodeEditorDataTempCustomization::MakeInstance()
{
	return MakeShareable(new FHeartGraphNodeEditorDataTempCustomization);
}

FHeartGraphNodeEditorDataTempCustomization::FHeartGraphNodeEditorDataTempCustomization()
{
	const FHeartEditorModule& HeartEditorModule = FModuleManager::LoadModuleChecked<FHeartEditorModule>("HeartEditor");

	Options.Add("GraphDefault");
	Options.Append(HeartEditorModule.GetSlateStyles());
}

void FHeartGraphNodeEditorDataTempCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
																 FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FHeartGraphNodeEditorDataTempCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
																   IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	uint32 Children;
	StructPropertyHandle->GetNumChildren(Children);

	for (uint32 i = 0; i < Children; ++i)
	{
		TSharedPtr<IPropertyHandle> Prop = StructPropertyHandle->GetChildHandle(i);

		if (Prop->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED(FHeartGraphNodeEditorDataTemp, EditorSlateStyle))
		{
			StyleProp = Prop;

			FDetailWidgetRow& EditorSlateStyleRow = StructBuilder.AddCustomRow(LOCTEXT("EditorSlateStyleSearchString", "Editor Slate Style"));

			EditorSlateStyleRow.NameContent()
				[
					Prop->CreatePropertyNameWidget()
				];

			EditorSlateStyleRow.ValueContent()
				[
					SNew(SComboBox<FName>)
					.OptionsSource(&Options)
					.OnSelectionChanged(this, &FHeartGraphNodeEditorDataTempCustomization::OnStyleSelectionChanged)
					.OnGenerateWidget(this, &FHeartGraphNodeEditorDataTempCustomization::OnGenerateStyleWidget)
					[
						SNew(STextBlock)
							.Text(this, &FHeartGraphNodeEditorDataTempCustomization::GetSelectedStyle)
					]
				];
		}
		else
		{
			StructBuilder.AddProperty(Prop.ToSharedRef());
		}
	}
}

void FHeartGraphNodeEditorDataTempCustomization::OnStyleSelectionChanged(const FName Name, ESelectInfo::Type SelectInfo)
{
	if (StyleProp.IsValid())
	{
		StyleProp->SetValue(Name);
	}
}

TSharedRef<SWidget> FHeartGraphNodeEditorDataTempCustomization::OnGenerateStyleWidget(const FName Style)
{
	FText Tooltip;

	if (Style == FName("GraphDefault"))
	{
		Tooltip = LOCTEXT("DefaultOptionTooltip", "Use the Default Editor Style of the graph's Schema");
	}

	return SNew(STextBlock)
		.Text(FText::FromString(FName::NameToDisplayString(Style.ToString(), false)))
		.ToolTipText(Tooltip);
}

FText FHeartGraphNodeEditorDataTempCustomization::GetSelectedStyle() const
{
	FName Value;
	if (StyleProp.IsValid())
	{
		StyleProp->GetValue(Value);
	}
	return FText::FromString(FName::NameToDisplayString(Value.ToString(), false));
}

#undef LOCTEXT_NAMESPACE

TSharedRef<IDetailCustomization> FHeartGraphNodeCustomization::MakeInstance()
{
	return MakeShareable(new FHeartGraphNodeCustomization);
}

void FHeartGraphNodeCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
}
