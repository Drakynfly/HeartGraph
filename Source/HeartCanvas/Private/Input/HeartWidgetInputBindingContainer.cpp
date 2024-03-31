// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartWidgetInputBindingContainer.h"
#include "Input/HeartInputBindingAsset.h"
#include "Input/HeartWidgetInputLinker.h"
#include "Components/Widget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartWidgetInputBindingContainer)

void FHeartWidgetInputBindingContainer::SetupLinker(UWidget* InOuter)
{
	check(InOuter);

	// Unbind everything from the current linker
	if (IsValid(Linker))
	{
		for (auto&& BindingAsset : BindingAssets)
        {
        	if (!IsValid(BindingAsset)) continue;

			Linker->RemoveBindings(BindingAsset->BindingData);
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

			Linker->AddBindings(BindingAsset->BindingData);
		}
	}
}