// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"

class FHeartGraphSchemaCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	FHeartGraphSchemaCustomization();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	void OnStyleSelectionChanged(FName Name, ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> OnGenerateStyleWidget(FName Style);
	FText GetSelectedStyle() const;

	TSharedPtr<IPropertyHandle> StyleProp;

	TArray<FName> StyleOptions;
};