﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartInputLinkerBase.h"

#include "HeartWidgetInputLinker.generated.h"

class UWidget;
class UDragDropOperation;

/**
 * Binds to UWidgets and externalizes input functions. This is used to set the DefaultLinkerClass in a
 * FHeartWidgetInputBindingContainer property. It can be subclassed if necessary for additional custom features, but
 * that is not usually needed.
 */
UCLASS(BlueprintType)
class HEARTCANVAS_API UHeartWidgetInputLinker : public UHeartInputLinkerBase
{
	GENERATED_BODY()

public:
	// Regular mouse / keyboard / game-pad events
	virtual FReply HandleOnMouseWheel(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	virtual FReply HandleOnMouseButtonDown(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	virtual FReply HandleOnMouseButtonUp(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	virtual FReply HandleOnKeyDown(UWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent);
	virtual FReply HandleOnKeyUp(UWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent);

	// Drag drop events
	virtual UDragDropOperation* HandleOnDragDetected(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	virtual bool HandleOnDrop(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
	virtual bool HandleOnDragOver(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
	virtual void HandleOnDragEnter(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
	virtual void HandleOnDragLeave(UWidget* Widget, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
	virtual void HandleOnDragCancelled(UWidget* Widget, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
};

template <>
struct THeartInputLinkerType<UWidget>
{
	static constexpr bool Supported = true;

	using FReplyType = FReply;
	using FValueType = UWidget*;
	using FDDOType = UDragDropOperation*;

	static FReplyType NoReply() { return FReply::Unhandled(); }

	HEARTCANVAS_API static UHeartWidgetInputLinker* FindLinker(const UWidget* Widget);
};

/**
 * Place this macro after the GENERATED_BODY or constructor of a UWidget derived class you wish to implement an InputLinker on.
 */
#define HEART_UMG_INPUT_LINKER_HEADER()\
protected:\
virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;\
virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;\
virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;\
virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;\
virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;\
virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;\
virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;\
virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;\
virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;\
virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;\
virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;\
public:

/**
 * Place this macro in the .cpp file for the class you put HEART_WIDGET_INPUT_LINKER_HEADER in.
 */
#define HEART_UMG_INPUT_LINKER_BODY(type)\
FReply type::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)\
{\
	auto&& Reply = Heart::Input::LinkOnMouseWheel<UWidget>(this, InGeometry, InMouseEvent);\
	return Reply.IsEventHandled() ? Reply : Super::NativeOnMouseWheel(InGeometry, InMouseEvent);\
}\
\
FReply type::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)\
{\
	auto&& Reply = Heart::Input::LinkOnMouseButtonDown<UWidget>(this, InGeometry, InMouseEvent);\
	return Reply.IsEventHandled() ? Reply : Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);\
}\
\
FReply type::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)\
{\
	auto&& Reply = Heart::Input::LinkOnMouseButtonUp<UWidget>(this, InGeometry, InMouseEvent);\
	return Reply.IsEventHandled() ? Reply : Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);\
}\
\
FReply type::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)\
{\
	auto&& Reply = Heart::Input::LinkOnKeyDown<UWidget>(this, InGeometry, InKeyEvent);\
	return Reply.IsEventHandled() ? Reply : Super::NativeOnKeyDown(InGeometry, InKeyEvent);\
}\
\
FReply type::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)\
{\
	auto&& Reply = Heart::Input::LinkOnKeyUp<UWidget>(this, InGeometry, InKeyEvent);\
	return Reply.IsEventHandled() ? Reply : Super::NativeOnKeyUp(InGeometry, InKeyEvent);\
}\
\
void type::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)\
{\
	OutOperation = Heart::Input::LinkOnDragDetected<UWidget>(this, InGeometry, InMouseEvent);\
	if (!IsValid(OutOperation))\
	{\
		Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);\
	}\
}\
\
bool type::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)\
{\
	bool Handled = Heart::Input::LinkOnDrop<UWidget, bool>(this, InGeometry, InDragDropEvent, InOperation);\
	return Handled ? true : Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);\
}\
\
bool type::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)\
{\
	bool Handled = Heart::Input::LinkOnDragOver<UWidget, bool>(this, InGeometry, InDragDropEvent, InOperation);\
	return Handled ? true : Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);\
}\
\
void type::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)\
{\
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);\
	Heart::Input::LinkOnDragEnter<UWidget>(this, InGeometry, InDragDropEvent, InOperation);\
}\
\
void type::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)\
{\
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);\
	Heart::Input::LinkOnDragLeave<UWidget>(this, InDragDropEvent, InOperation);\
}\
\
void type::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)\
{\
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);\
	Heart::Input::LinkOnDragCancelled<UWidget>(this, InDragDropEvent, InOperation);\
}