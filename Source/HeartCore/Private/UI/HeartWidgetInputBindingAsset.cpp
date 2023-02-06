// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputBindingAsset.h"

void UHeartWidgetInputBindingAsset::BindLinker(const TObjectPtr<UHeartWidgetInputLinker>& Linker)
{
	for (auto&& Binding : BindingData)
	{
		if (!IsValid(Binding.InputHandler)) continue;

		Binding.InputHandler->Bind(Linker, Binding.Triggers);
	}
}

void UHeartWidgetInputBindingAsset::UnbindLinker(const TObjectPtr<UHeartWidgetInputLinker>& Linker)
{
	for (auto&& Binding : BindingData)
	{
		if (!IsValid(Binding.InputHandler)) continue;

		Binding.InputHandler->Unbind(Linker, Binding.Triggers);
	}
}
