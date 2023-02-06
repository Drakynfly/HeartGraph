// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputBindingContainer.h"
#include "UI/HeartWidgetInputBindingAsset.h"

void FHeartWidgetInputBindingContainer::SetupLinker(UWidget* InOuter)
{
	check(InOuter);
	Outer = InOuter;

	// Unbind everything from the current linker
	if (IsValid(Linker))
	{
		for (auto&& BindingAsset : BindingAssets)
        {
        	if (!IsValid(BindingAsset)) continue;

			BindingAsset->UnbindLinker(Linker);
        }

		Linker = nullptr;
	}

	if (IsValid(DefaultLinkerClass))
	{
		Linker = NewObject<UHeartWidgetInputLinker>(InOuter, DefaultLinkerClass);

		// Rebind everything to the new linker
		for (auto&& BindingAsset : BindingAssets)
		{
			if (!IsValid(BindingAsset)) continue;

			BindingAsset->BindLinker(Linker);
		}
	}
}
