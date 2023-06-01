// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"

class FHeartGraphNodeEditorDataTempCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	FHeartGraphNodeEditorDataTempCustomization();

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:
	void OnStyleSelectionChanged(FName Name, ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> OnGenerateStyleWidget(FName Style);
	FText GetSelectedStyle() const;

	TSharedPtr<IPropertyHandle> StyleProp;

	TArray<FName> Options;
};

class FHeartGraphNodeCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
