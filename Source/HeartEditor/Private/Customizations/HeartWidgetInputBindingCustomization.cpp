// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartWidgetInputBindingCustomization.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyEditing.h"
#include "UI/HeartWidgetInputBindingAsset.h"

#define LOCTEXT_NAMESPACE "ItemsArrayCustomization"

TSharedRef<IPropertyTypeCustomization> FHeartWidgetInputBindingCustomization::MakeInstance()
{
	return MakeShareable(new FHeartWidgetInputBindingCustomization);
}

void FHeartWidgetInputBindingCustomization::CustomizeHeader(const TSharedRef<IPropertyHandle> PropertyHandle,
                                                   FDetailWidgetRow& HeaderRow,
                                                   IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	auto&& InputHandlerHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FHeartWidgetInputBinding, InputHandler));
	auto&& TriggersHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FHeartWidgetInputBinding, Triggers));

	HeaderRow.NameContent()
		[
			InputHandlerHandle->CreatePropertyValueWidget()
		];

	HeaderRow.ValueContent()
		[
			TriggersHandle->CreatePropertyValueWidget()
		];
}

void FHeartWidgetInputBindingCustomization::CustomizeChildren(const TSharedRef<IPropertyHandle> StructPropertyHandle,
                                                     IDetailChildrenBuilder& StructBuilder,
                                                     IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	const TSharedPtr<IPropertyHandle> TriggersHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FHeartWidgetInputBinding, Triggers));
	check(TriggersHandle.IsValid())

	const TSharedRef<FDetailArrayBuilder> ArrayBuilder = MakeShareable(
		new FDetailArrayBuilder(TriggersHandle.ToSharedRef(), false, true, false));

	ArrayBuilder->OnGenerateArrayElementWidget(FOnGenerateArrayElementWidget::CreateSP(this, &FHeartWidgetInputBindingCustomization::OnGenerateElement));

	StructBuilder.AddCustomBuilder(ArrayBuilder);
}

void FHeartWidgetInputBindingCustomization::OnGenerateElement(TSharedRef<IPropertyHandle> ElementProperty, int32 ElementIndex,
												 IDetailChildrenBuilder& ChildrenBuilder)
{
	ChildrenBuilder.AddProperty(ElementProperty);
}

#undef LOCTEXT_NAMESPACE