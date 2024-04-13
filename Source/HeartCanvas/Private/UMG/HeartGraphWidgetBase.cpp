// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphWidgetBase.h"
#include "HeartCanvasPaletteCategory.h"
#include "Input/HeartDragDropOperation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphWidgetBase)

UHeartGraphWidgetBase::UHeartGraphWidgetBase(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
	// Allow keyboard to trigger input on all graph widgets. (This can always be turned off when not desired)
	SetIsFocusable(true);
}

HEART_UMG_INPUT_LINKER_BODY(UHeartGraphWidgetBase)

#if WITH_EDITOR
const FText UHeartGraphWidgetBase::GetPaletteCategory()
{
	return Heart::Canvas::PaletteCategory::Default;
}
#endif

UHeartInputLinkerBase* UHeartGraphWidgetBase::ResolveLinker_Implementation() const
{
	// Assume that something up our parent chain will be able to handle this.
	return THeartInputLinkerType<UWidget>::FindLinker(GetTypedOuter<UWidget>());
}