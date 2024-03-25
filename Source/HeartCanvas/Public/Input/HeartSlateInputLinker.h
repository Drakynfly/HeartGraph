// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartInputLinkerBase.h"
#include "HeartSlateInputLinker.generated.h"

struct FHeartManualEvent;

UCLASS()
class HEARTCANVAS_API UHeartSlateInputLinker : public UHeartInputLinkerBase
{
	GENERATED_BODY()

protected:
	TOptional<FReply> TryCallbacks(const Heart::Input::FInputTrip& Trip, const TSharedRef<SWidget>& Widget, const FHeartInputActivation& Activation);

public:
	// Regular mouse / keyboard / game-pad events
	virtual FReply HandleOnMouseWheel(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	virtual FReply HandleOnMouseButtonDown(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	virtual FReply HandleOnMouseButtonUp(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	virtual FReply HandleOnKeyDown(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent);
	virtual FReply HandleOnKeyUp(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent);

	// @todo slate DDO support

	// Custom input
	virtual FReply HandleManualInput(const TSharedRef<SWidget>& Widget, /*const FGeometry& InGeometry,*/ FName Key, const FHeartManualEvent& Activation);
	TArray<FHeartManualInputQueryResult> QueryManualTriggers(const TSharedRef<SWidget>& Widget) const;
};

namespace Heart::Input
{
	template <>
	struct TLinkerType<SWidget>
	{
		static constexpr bool Supported = true;

		using FValueType = const TSharedRef<SWidget>&;
		// DDO type // using FDDOType = const TSharedRef<SWidget>&;

		using FDescriptionDelegate = TSpecifiedDelegate<TDelegate<FText(const TSharedRef<SWidget>&)>>;
		using FConditionDelegate = TSpecifiedDelegate<TDelegate<bool(const TSharedRef<SWidget>&)>>;
		using FHandlerDelegate = TSpecifiedDelegate<TDelegate<FReply(TSharedRef<SWidget>&, const FHeartInputActivation&)>>;
		// DDO handler

		HEARTCANVAS_API static UHeartSlateInputLinker* FindLinker(const TSharedRef<SWidget>& Widget);
	};
}

/**
 * Place this macro after the SLATE_END_ARGS or Construct of a SWidget derived class you wish to implement an InputLinker on.
 */
#define HEART_SLATE_INPUT_LINKER_HEADER(SuperType)\
using Super = SuperType;\
protected:\
virtual FReply OnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;\
virtual FReply OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;\
virtual FReply OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;\
virtual FReply OnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;\
virtual FReply OnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;\
virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;\
virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;\
virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;\
virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;\
virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;\
public: