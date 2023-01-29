// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputBindingContainer.h"
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
        	if (IsValid(BindingAsset))
        	{
	            for (auto&& Binding : BindingAsset->BindingData)
	            {
	            	Binding.BindingObject->Unbind(Linker, Binding.Triggers);
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
			if (IsValid(BindingAsset))
			{
				for (auto&& Binding : BindingAsset->BindingData)
				{
					Binding.BindingObject->Bind(Linker, Binding.Triggers);
				}
			}
		}
	}
}
