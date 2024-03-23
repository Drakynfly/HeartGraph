// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Components/Widget.h"

#include "HeartDragDropOperation.h"
#include "HeartInputTypes.h"
#include "HeartWidgetInputLinkerRedirector.h"
#include "Input/HeartInputTrip.h"
#include "HeartWidgetInputLinker.generated.h"


struct FHeartWidgetInputBinding;
struct FHeartManualEvent;

USTRUCT(BlueprintType)
struct FHeartManualInputQueryResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ManualInputQueryResult")
	FName Key;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ManualInputQueryResult")
	FText Description;
};


/**
 * Binds to UWidgets and externalizes input functions. This is used to set the DefaultLinkerClass in a
 * FHeartWidgetInputBindingContainer property. It can be subclassed if necessary for additional custom features, but
 * that is not usually needed.
 */
UCLASS(BlueprintType)
class HEARTCORE_API UHeartWidgetInputLinker : public UObject
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
	virtual UHeartDragDropOperation* HandleOnDragDetected(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	virtual bool HandleNativeOnDrop(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
	virtual bool HandleNativeOnDragOver(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
	virtual void HandleNativeOnDragEnter(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
	virtual void HandleNativeOnDragLeave(UWidget* Widget, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
	virtual void HandleNativeOnDragCancelled(UWidget* Widget, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);

	// Custom input
	virtual FReply HandleManualInput(UWidget* Widget, /*const FGeometry& InGeometry,*/ FName Key, const FHeartManualEvent& Activation);
	TArray<FHeartManualInputQueryResult> QueryManualTriggers(const UWidget* Widget) const;

public:
	void BindInputCallback(const Heart::Input::FInputTrip& Trip, const Heart::Input::FConditionalInputCallback& InputCallback);
	void UnbindInputCallback(const Heart::Input::FInputTrip& Trip);

	void BindToOnDragDetected(const Heart::Input::FInputTrip& Trip, const Heart::Input::FConditionalDragDropTrigger& DragDropTrigger);
	void UnbindToOnDragDetected(const Heart::Input::FInputTrip& Trip);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetInputLinker")
	void AddBindings(const TArray<FHeartWidgetInputBinding>& Bindings);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetInputLinker")
	void RemoveBindings(const TArray<FHeartWidgetInputBinding>& Bindings);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetInputLinker")
	bool TriggerManualInput(UWidget* Widget, FName Key, const FHeartManualEvent& Activation);

private:
	// Input trips that fire a delegate.
	TMultiMap<Heart::Input::FInputTrip, Heart::Input::FConditionalInputCallback> InputCallbackMappings;

	// Input trips that begin a drag drop operation
	TMultiMap<Heart::Input::FInputTrip, Heart::Input::FConditionalDragDropTrigger> DragDropTriggers;
};

namespace Heart::Input
{
	static UHeartWidgetInputLinker* FindLinkerForWidget(const UWidget* Widget)
	{
		if (!ensure(IsValid(Widget))) return nullptr;

		for (auto&& Test = Widget; Test; Test = Test->GetTypedOuter<UWidget>())
		{
			if (Test->Implements<UHeartWidgetInputLinkerRedirector>())
			{
				// In some cases, a widget may implement the interface but have linking disabled, and return nullptr
				if (UHeartWidgetInputLinker* Linker = IHeartWidgetInputLinkerRedirector::Execute_ResolveLinker(Test))
				{
					return Linker;
				}
			}
		}

		return nullptr;
	}

	template <typename TWidget>
	static FReply LinkOnMouseWheel(TWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		static_assert(TIsDerivedFrom<TWidget, UWidget>::Value, TEXT("TWidget must be a UWidget class!"));

		if (auto&& Linker = FindLinkerForWidget(Widget))
		{
			FReply BindingReply = Linker->HandleOnMouseWheel(Widget, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return FReply::Unhandled();
	}

	template <typename TWidget>
	static FReply LinkOnMouseButtonDown(TWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		static_assert(TIsDerivedFrom<TWidget, UWidget>::Value, TEXT("TWidget must be a UWidget class!"));

		if (auto&& Linker = FindLinkerForWidget(Widget))
		{
			FReply BindingReply = Linker->HandleOnMouseButtonDown(Widget, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return FReply::Unhandled();
	}

	template <typename TWidget>
	static FReply LinkOnMouseButtonUp(TWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		static_assert(TIsDerivedFrom<TWidget, UWidget>::Value, TEXT("TWidget must be a UWidget class!"));

		if (auto&& Linker = FindLinkerForWidget(Widget))
		{
			FReply BindingReply = Linker->HandleOnMouseButtonUp(Widget, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return FReply::Unhandled();
	}

	template <typename TWidget>
	static FReply LinkOnKeyDown(TWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
	{
		static_assert(TIsDerivedFrom<TWidget, UWidget>::Value, TEXT("TWidget must be a UWidget class!"));

		if (auto&& Linker = FindLinkerForWidget(Widget))
		{
			FReply BindingReply = Linker->HandleOnKeyDown(Widget, InGeometry, InKeyEvent);

			if (BindingReply.IsEventHandled())
	        {
        		return BindingReply;
	        }
		}

		return FReply::Unhandled();
	}

	template <typename TWidget>
	static FReply LinkOnKeyUp(TWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
	{
		static_assert(TIsDerivedFrom<TWidget, UWidget>::Value, TEXT("TWidget must be a UWidget class!"));

		if (auto&& Linker = FindLinkerForWidget(Widget))
		{
			FReply BindingReply = Linker->HandleOnKeyUp(Widget, InGeometry, InKeyEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return FReply::Unhandled();
	}

	template <typename TWidget>
	static bool LinkOnDragDetected(TWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	                               UDragDropOperation*& OutOperation)
	{
		static_assert(TIsDerivedFrom<TWidget, UWidget>::Value, TEXT("TWidget must be a UWidget class!"));

		if (auto&& Linker = FindLinkerForWidget(Widget))
		{
			if (auto&& LinkerOperation = Linker->HandleOnDragDetected(Widget, InGeometry, InMouseEvent))
			{
				OutOperation = LinkerOperation;
				return true;
			}
		}

		return false;
	}

	template <typename TWidget>
	static bool LinkOnDrop(TWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
						   UDragDropOperation* InOperation)
	{
		static_assert(TIsDerivedFrom<TWidget, UWidget>::Value, TEXT("TWidget must be a UWidget class!"));

		if (auto&& Linker = FindLinkerForWidget(Widget))
		{
			if (Linker->HandleNativeOnDrop(Widget, InGeometry, InDragDropEvent, InOperation))
			{
				return true;
			}
		}

		return false;
	}

	template <typename TWidget>
	static bool LinkOnDragOver(TWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
							   UDragDropOperation* InOperation)
	{
		static_assert(TIsDerivedFrom<TWidget, UWidget>::Value, TEXT("TWidget must be a UWidget class!"));

		if (auto&& Linker = FindLinkerForWidget(Widget))
		{
			if (Linker->HandleNativeOnDragOver(Widget, InGeometry, InDragDropEvent, InOperation))
			{
				return true;
			}
		}

		return false;
	}

	template <typename TWidget>
	static void LinkOnDragEnter(TWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
								UDragDropOperation* InOperation)
	{
		static_assert(TIsDerivedFrom<TWidget, UWidget>::Value, TEXT("TWidget must be a UWidget class!"));

		if (auto&& Linker = FindLinkerForWidget(Widget))
		{
			Linker->HandleNativeOnDragEnter(Widget, InGeometry, InDragDropEvent, InOperation);
		}
	}

	template <typename TWidget>
	static void LinkOnDragLeave(TWidget* Widget, const FDragDropEvent& InDragDropEvent,
								UDragDropOperation* InOperation)
	{
		static_assert(TIsDerivedFrom<TWidget, UWidget>::Value, TEXT("TWidget must be a UWidget class!"));

		if (auto&& Linker = FindLinkerForWidget(Widget))
		{
			Linker->HandleNativeOnDragLeave(Widget, InDragDropEvent, InOperation);
		}
	}

	template <typename TWidget>
	static void LinkOnDragCancelled(TWidget* Widget, const FDragDropEvent& InDragDropEvent,
									UDragDropOperation* InOperation)
	{
		static_assert(TIsDerivedFrom<TWidget, UWidget>::Value, TEXT("TWidget must be a UWidget class!"));

		if (auto&& Linker = FindLinkerForWidget(Widget))
		{
			Linker->HandleNativeOnDragCancelled(Widget, InDragDropEvent, InOperation);
		}
	}
}

/**
 * Place this macro after the GENERATED_BODY or constructor of a UWidget derived class you wish to implement an InputLinker on.
 */
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

/**
 * Place this macro in the .cpp file for the class you put HEART_WIDGET_INPUT_LINKER_HEADER in.
 */
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