// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphWidgetBase.h"
#include "HeartCanvasPaletteCategory.h"
#include "Components/PanelWidget.h"

UHeartGraphWidgetBase::UHeartGraphWidgetBase(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
	// Allow keyboard to trigger input on all graph widgets. (This can always be turned off when not desired)
	bIsFocusable = true;
}

HEART_WIDGET_INPUT_LINKER_BODY(UHeartGraphWidgetBase)

#if WITH_EDITOR
const FText UHeartGraphWidgetBase::GetPaletteCategory()
{
	return Heart::Canvas::PaletteCategory::Default;
}
#endif

UHeartWidgetInputLinker* UHeartGraphWidgetBase::ResolveLinker_Implementation() const
{
	for (auto&& Parent = GetOuter(); Parent; Parent = Parent->GetOuter())
	{
		if (Parent->Implements<UHeartWidgetInputLinkerRedirector>())
		{
			return Execute_ResolveLinker(Parent);
		}
	}

	return nullptr;
}

void UHeartGraphWidgetBase::GetWidgetActions() const
{
	if (auto&& Linker = ResolveLinker())
	{

	}
}