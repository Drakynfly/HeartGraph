// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartGuidCustomization.h"

#include "DetailWidgetRow.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "HeartGuidCustomization"

TSharedRef<IPropertyTypeCustomization> FHeartGuidCustomization::MakeInstance()
{
	return MakeShared<FHeartGuidCustomization>();
}

void FHeartGuidCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
											  FDetailWidgetRow& HeaderRow,
											  IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	PropertyHandle = StructPropertyHandle;

	HeaderRow
		.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.HAlign(HAlign_Center)
		.MinDesiredWidth(325.0f)
		.MaxDesiredWidth(325.0f)
		[
			// text box
			SNew(STextBlock)
				.ColorAndOpacity(FAppStyle::Get().GetSlateColor("Colors.Foreground"))
				.Font(IPropertyTypeCustomizationUtils::GetRegularFont())
				.Text(this, &FHeartGuidCustomization::HandleTextBoxText)
		];
}

void FHeartGuidCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
												IDetailChildrenBuilder& StructBuilder,
												IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// do nothing
}

FText FHeartGuidCustomization::HandleTextBoxText() const
{
	TArray<void*> RawData;
	PropertyHandle->AccessRawData(RawData);

	if (RawData.Num() != 1)
	{
		return LOCTEXT("MultipleValues", "Multiple Values");
	}

	if (RawData[0] == nullptr)
	{
		return FText::GetEmpty();
	}

	return FText::FromString(static_cast<FGuid*>(RawData[0])->ToString(EGuidFormats::Short));
}

#undef LOCTEXT_NAMESPACE