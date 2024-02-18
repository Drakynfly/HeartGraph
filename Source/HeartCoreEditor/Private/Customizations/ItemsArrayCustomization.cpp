// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Customizations/ItemsArrayCustomization.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "ItemsArrayCustomization"

TSharedRef<IPropertyTypeCustomization> Heart::FItemsArrayCustomization::MakeInstance()
{
	return MakeInstance(FArgs());
}

TSharedRef<IPropertyTypeCustomization> Heart::FItemsArrayCustomization::MakeInstance(FArgs Args)
{
	return MakeShareable(new FItemsArrayCustomization(Args));
}

void Heart::FItemsArrayCustomization::CustomizeHeader(const TSharedRef<IPropertyHandle> PropertyHandle,
													  FDetailWidgetRow& HeaderRow,
													  IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if (!Customization.HeaderProp.IsNone())
	{
		HeaderRow.NameContent()
		[
			PropertyHandle->GetChildHandle(Customization.HeaderProp)->CreatePropertyValueWidget()
		];
	}
	else
	{
		HeaderRow.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		];
	}

	TSharedPtr<IPropertyHandle> ValueContextHandle;
	if (!Customization.ArrayProp.IsNone())
	{
		ValueContextHandle = PropertyHandle->GetChildHandle(Customization.ArrayProp);
	}
	else
	{
		ValueContextHandle = PropertyHandle->GetChildHandle(0);
	}

	HeaderRow.ValueContent()
		[
			ValueContextHandle->CreatePropertyValueWidget()
		];
}

void Heart::FItemsArrayCustomization::CustomizeChildren(const TSharedRef<IPropertyHandle> StructPropertyHandle,
                                                        IDetailChildrenBuilder& StructBuilder,
                                                        IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> ArrayHandle;

	if (!Customization.ArrayProp.IsNone())
	{
		ArrayHandle = StructPropertyHandle->GetChildHandle(Customization.ArrayProp);
	}
	else
	{
		ArrayHandle = StructPropertyHandle->GetChildHandle(0);
	}

	check(ArrayHandle.IsValid())

	const TSharedRef<FDetailArrayBuilder> ArrayBuilder = MakeShareable(
		new FDetailArrayBuilder(ArrayHandle.ToSharedRef(), false, true, false));

	ArrayBuilder->OnGenerateArrayElementWidget(FOnGenerateArrayElementWidget::CreateSP(this, &FItemsArrayCustomization::OnGenerateElement));

	StructBuilder.AddCustomBuilder(ArrayBuilder);
}

void Heart::FItemsArrayCustomization::OnGenerateElement(TSharedRef<IPropertyHandle> ElementProperty, int32 ElementIndex,
														IDetailChildrenBuilder& ChildrenBuilder)
{
	ChildrenBuilder.AddProperty(ElementProperty);
}

#undef LOCTEXT_NAMESPACE