// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "IPropertyTypeCustomization.h"

namespace Heart
{
	/**
	 * Utility customization for any struct that should appear as an array.
	 */
	class HEARTCOREEDITOR_API FItemsArrayCustomization final : public IPropertyTypeCustomization
	{
	public:
		struct FArgs
        {
        	FName HeaderProp = NAME_None;
        	FName ArrayProp = NAME_None;
        };

	private:
		friend SharedPointerInternals::TIntrusiveReferenceController<FItemsArrayCustomization, ESPMode::ThreadSafe>;

		FItemsArrayCustomization(const FArgs& Args)
		  : Customization(Args) {}

	public:
		static TSharedRef<IPropertyTypeCustomization> MakeInstance();
		static TSharedRef<IPropertyTypeCustomization> MakeInstance(FArgs Args);

		virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
		virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

		void OnGenerateElement(TSharedRef<IPropertyHandle> ElementProperty, int32 ElementIndex, IDetailChildrenBuilder& ChildrenBuilder);

	private:
		FArgs Customization;
	};
}
