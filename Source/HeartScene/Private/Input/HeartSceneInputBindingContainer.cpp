// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartSceneInputBindingContainer.h"
#include "Input/HeartInputBindingAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSceneInputBindingContainer)

void FHeartSceneInputBindingContainer::SetupLinker(UObject* InOuter)
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
		Linker = NewObject<UHeartSceneInputLinker>(InOuter, DefaultLinkerClass);

		// Rebind everything to the new linker
		for (auto&& BindingAsset : BindingAssets)
		{
			if (!IsValid(BindingAsset)) continue;

			Linker->AddBindings(BindingAsset->BindingData);
		}
	}
}