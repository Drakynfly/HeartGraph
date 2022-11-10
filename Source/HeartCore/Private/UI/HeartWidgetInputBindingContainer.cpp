// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputBindingContainer.h"
#include "Blueprint/UserWidget.h"
#include "UI/HeartWidgetInputBindingAsset.h"
#include "UI/HeartWidgetInputBindingBase.h"

void FHeartWidgetInputBindingContainer::SetupLinker(UWidget* InOuter)
{
	check(InOuter);
	Outer = InOuter;

	// Unbind everything from the current linker
	if (Linker)
	{
		for (auto&& BindingAsset : BindingAssets)
        {
        	if (ensure(IsValid(BindingAsset)))
        	{
	            for (auto&& Binding : BindingAsset->Bindings)
	            {
	            	Binding->Unbind(Linker);
	            }
        	}
        }

		Linker = nullptr;
	}

	if (IsValid(DefaultLinkerClass))
	{
		Linker = NewObject<UHeartWidgetInputLinker>(InOuter, DefaultLinkerClass);

		// Rebind everything to the new linker
		for (auto&& BindingAsset : BindingAssets)
		{
			if (ensure(IsValid(BindingAsset)))
			{
				for (auto&& Binding : BindingAsset->Bindings)
				{
					Binding->Bind(Linker);
				}
			}
		}
	}
}
