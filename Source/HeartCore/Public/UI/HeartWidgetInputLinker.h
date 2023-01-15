// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartDragDropOperation.h"
#include "HeartInputTypes.h"
#include "HeartWidgetInputEvent.h"
#include "HeartWidgetInputLinkerRedirector.h"
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
	FReply HandleOnMouseWheel(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	FReply HandleOnMouseButtonDown(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	FReply HandleOnMouseButtonUp(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	FReply HandleOnKeyDown(UWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent);
	FReply HandleOnKeyUp(UWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent);

	UHeartDragDropOperation* HandleOnDragDetected(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	bool HandleNativeOnDrop(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
	bool HandleNativeOnDragOver(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
	void HandleNativeOnDragEnter(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
	void HandleNativeOnDragLeave(UWidget* Widget, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
	void HandleNativeOnDragCancelled(UWidget* Widget, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);

public:
	void BindInputCallback(const FHeartWidgetInputTrip& Trip, const Heart::Input::FConditionalInputCallback& InputCallback);
	void UnbindInputCallback(const FHeartWidgetInputTrip& Trip);

	void BindToOnDragDetected(const FHeartWidgetInputTrip& Trip, const Heart::Input::FConditionalDragDropTrigger& DragDropTrigger);
	void UnbindToOnDragDetected(const FHeartWidgetInputTrip& Trip);

private:
	TMultiMap<FHeartWidgetInputTrip, Heart::Input::FConditionalInputCallback> InputCallbackMappings;

	// Keys that trip a drag drop operation, paired the the class of DDO and the widget class to spawn as a visual
	TMultiMap<FHeartWidgetInputTrip, Heart::Input::FConditionalDragDropTrigger> DragDropTriggers;
};

namespace Heart::Input
{
	static UHeartWidgetInputLinker* GetWidgetLinker(const UWidget* Widget)
	{
		if (Widget && Widget->Implements<UHeartWidgetInputLinkerRedirector>())
		{
			return IHeartWidgetInputLinkerRedirector::Execute_ResolveLinker(Widget);
		}

		UE_LOG(LogTemp, Warning, TEXT("Widget class %s does not implement IHeartWidgetInputLinkerRedirector! Linker input will not function."), *Widget->GetClass()->GetName())
		return nullptr;
	}

	template <typename TWidget,
		typename = typename TEnableIf<TIsDerivedFrom<TWidget, UWidget>::IsDerived, TWidget>::Type>
	static FReply LinkOnMouseWheel(TWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = GetWidgetLinker(Widget))
		{
			FReply BindingReply = Linker->HandleOnMouseWheel(Widget, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return FReply::Unhandled();
	}

	template <typename TWidget,
		typename = typename TEnableIf<TIsDerivedFrom<TWidget, UWidget>::IsDerived, TWidget>::Type>
	static FReply LinkOnMouseButtonDown(TWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = GetWidgetLinker(Widget))
		{
			FReply BindingReply = Linker->HandleOnMouseButtonDown(Widget, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return FReply::Unhandled();
	}

	template <typename TWidget,
		typename = typename TEnableIf<TIsDerivedFrom<TWidget, UWidget>::IsDerived, TWidget>::Type>
	static FReply LinkOnMouseButtonUp(TWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = GetWidgetLinker(Widget))
		{
			FReply BindingReply = Linker->HandleOnMouseButtonUp(Widget, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return FReply::Unhandled();
	}

	template <typename TWidget,
		typename = typename TEnableIf<TIsDerivedFrom<TWidget, UWidget>::IsDerived, TWidget>::Type>
	static FReply LinkOnKeyDown(TWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
	{
		if (auto&& Linker = GetWidgetLinker(Widget))
		{
			FReply BindingReply = Linker->HandleOnKeyDown(Widget, InGeometry, InKeyEvent);

			if (BindingReply.IsEventHandled())
	        {
        		return BindingReply;
	        }
		}

		return FReply::Unhandled();
	}

	template <typename TWidget,
		typename = typename TEnableIf<TIsDerivedFrom<TWidget, UWidget>::IsDerived, TWidget>::Type>
	static FReply LinkOnKeyUp(TWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
	{
		if (auto&& Linker = GetWidgetLinker(Widget))
		{
			FReply BindingReply = Linker->HandleOnKeyUp(Widget, InGeometry, InKeyEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return FReply::Unhandled();
	}

	template <typename TWidget,
		typename = typename TEnableIf<TIsDerivedFrom<TWidget, UWidget>::IsDerived, TWidget>::Type>
	static bool LinkOnDragDetected(TWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	                               UDragDropOperation*& OutOperation)
	{
		if (auto&& Linker = GetWidgetLinker(Widget))
		{
			if (auto&& LinkerOperation = Linker->HandleOnDragDetected(Widget, InGeometry, InMouseEvent))
			{
				OutOperation = LinkerOperation;
				return true;
			}
		}

		return false;
	}

	template <typename TWidget,
		typename = typename TEnableIf<TIsDerivedFrom<TWidget, UWidget>::IsDerived, TWidget>::Type>
	static bool LinkOnDrop(TWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
						   UDragDropOperation* InOperation)
	{
		if (auto&& Linker = GetWidgetLinker(Widget))
		{
			if (Linker->HandleNativeOnDrop(Widget, InGeometry, InDragDropEvent, InOperation))
			{
				return true;
			}
		}

		return false;
	}

	template <typename TWidget,
		typename = typename TEnableIf<TIsDerivedFrom<TWidget, UWidget>::IsDerived, TWidget>::Type>
	static bool LinkOnDragOver(TWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
							   UDragDropOperation* InOperation)
	{
		if (auto&& Linker = GetWidgetLinker(Widget))
		{
			if (Linker->HandleNativeOnDragOver(Widget, InGeometry, InDragDropEvent, InOperation))
			{
				return true;
			}
		}

		return false;
	}

	template <typename TWidget,
		typename = typename TEnableIf<TIsDerivedFrom<TWidget, UWidget>::IsDerived, TWidget>::Type>
	static void LinkOnDragEnter(TWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
								UDragDropOperation* InOperation)
	{
		if (auto&& Linker = GetWidgetLinker(Widget))
		{
			Linker->HandleNativeOnDragEnter(Widget, InGeometry, InDragDropEvent, InOperation);
		}
	}

	template <typename TWidget,
		typename = typename TEnableIf<TIsDerivedFrom<TWidget, UWidget>::IsDerived, TWidget>::Type>
	static void LinkOnDragLeave(TWidget* Widget, const FDragDropEvent& InDragDropEvent,
								UDragDropOperation* InOperation)
	{
		if (auto&& Linker = GetWidgetLinker(Widget))
		{
			Linker->HandleNativeOnDragLeave(Widget, InDragDropEvent, InOperation);
		}
	}

	template <typename TWidget,
		typename = typename TEnableIf<TIsDerivedFrom<TWidget, UWidget>::IsDerived, TWidget>::Type>
	static void LinkOnDragCancelled(TWidget* Widget, const FDragDropEvent& InDragDropEvent,
									UDragDropOperation* InOperation)
	{
		if (auto&& Linker = GetWidgetLinker(Widget))
		{
			Linker->HandleNativeOnDragCancelled(Widget, InDragDropEvent, InOperation);
		}
	}
}

#define HEART_WIDGET_INPUT_LINKER_HEADER()\
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
public:\


#define HEART_WIDGET_INPUT_LINKER_BODY(type)\
FReply type::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)\
{\
	auto&& Reply = Heart::Input::LinkOnMouseWheel<ThisClass>(this, InGeometry, InMouseEvent);\
	return Reply.IsEventHandled() ? Reply : Super::NativeOnMouseWheel(InGeometry, InMouseEvent);\
}\
\
FReply type::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)\
{\
	auto&& Reply = Heart::Input::LinkOnMouseButtonDown<ThisClass>(this, InGeometry, InMouseEvent);\
	return Reply.IsEventHandled() ? Reply : Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);\
}\
\
FReply type::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)\
{\
	auto&& Reply = Heart::Input::LinkOnMouseButtonUp<ThisClass>(this, InGeometry, InMouseEvent);\
	return Reply.IsEventHandled() ? Reply : Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);\
}\
\
FReply type::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)\
{\
	auto&& Reply = Heart::Input::LinkOnKeyDown<ThisClass>(this, InGeometry, InKeyEvent);\
	return Reply.IsEventHandled() ? Reply : Super::NativeOnKeyDown(InGeometry, InKeyEvent);\
}\
\
FReply type::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)\
{\
	auto&& Reply = Heart::Input::LinkOnKeyUp<ThisClass>(this, InGeometry, InKeyEvent);\
	return Reply.IsEventHandled() ? Reply : Super::NativeOnKeyUp(InGeometry, InKeyEvent);\
}\
\
void type::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)\
{\
	if (!Heart::Input::LinkOnDragDetected<ThisClass>(this, InGeometry, InMouseEvent, OutOperation))\
	{\
		Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);\
	}\
}\
\
bool type::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)\
{\
	bool Handled = Heart::Input::LinkOnDrop<ThisClass>(this, InGeometry, InDragDropEvent, InOperation);\
	return Handled ? true : Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);\
}\
\
bool type::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)\
{\
	bool Handled = Heart::Input::LinkOnDragOver<ThisClass>(this, InGeometry, InDragDropEvent, InOperation);\
	return Handled ? true : Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);\
}\
\
void type::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)\
{\
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);\
	Heart::Input::LinkOnDragEnter<ThisClass>(this, InGeometry, InDragDropEvent, InOperation);\
}\
\
void type::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)\
{\
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);\
	Heart::Input::LinkOnDragLeave<ThisClass>(this, InDragDropEvent, InOperation);\
}\
\
void type::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)\
{\
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);\
	Heart::Input::LinkOnDragCancelled<ThisClass>(this, InDragDropEvent, InOperation);\
}