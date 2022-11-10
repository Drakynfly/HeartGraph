// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartDragDropOperation.h"
#include "HeartWidgetInputCondition.h"
#include "HeartWidgetInputEvent.h"
#include "HeartWidgetInputTrip.h"
#include "HeartWidgetInputLinker.generated.h"

/**
 *
 */
UCLASS()
class HEARTCORE_API UHeartWidgetInputLinker : public UObject
{
	GENERATED_BODY()

public:
	struct FConditionalInputCallback
	{
		FHeartWidgetInputCondition Condition;
		FHeartWidgetLinkedInput Callback;
	};

	struct FConditionalDragDropTrigger
	{
		FHeartWidgetInputCondition Condition;
		TSubclassOf<UHeartDragDropOperation> Class;
		TSubclassOf<UUserWidget> VisualClass;
		EDragPivot Pivot;
		FVector2D Offset;
	};

	FReply HandleOnMouseWheel(UWidget* Widget, const FPointerEvent& PointerEvent);
	FReply HandleOnMouseButtonDown(UWidget* Widget, const FPointerEvent& PointerEvent);
	FReply HandleOnMouseButtonUp(UWidget* Widget, const FPointerEvent& PointerEvent);
	FReply HandleOnKeyDown(UWidget* Widget, const FKeyEvent& KeyEvent);
	FReply HandleOnKeyUp(UWidget* Widget, const FKeyEvent& KeyEvent);

	UHeartDragDropOperation* HandleOnDragDetected(UWidget* Widget, const FPointerEvent& PointerEvent);
	bool HandleNativeOnDrop(UWidget* Widget, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
	bool HandleNativeOnDragOver(UWidget* Widget, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);

public:
	void BindInputCallback(const FHeartWidgetInputTrip& Trip, const FConditionalInputCallback& InputCallback);
	void UnbindInputCallback(const FHeartWidgetInputTrip& Trip);

	void BindToOnDragDetected(const FHeartWidgetInputTrip& Trip, const FConditionalDragDropTrigger& DragDropTrigger);
	void UnbindToOnDragDetected(const FHeartWidgetInputTrip& Trip);

private:
	TMap<FHeartWidgetInputTrip, FConditionalInputCallback> InputCallbackMappings;

	// Keys that trip a drag drop operation, paired the the class of DDO and the widget class to spawn as a visual
	TMap<FHeartWidgetInputTrip, FConditionalDragDropTrigger> DragDropTriggers;
};
