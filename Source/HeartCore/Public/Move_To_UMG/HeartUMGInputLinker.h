// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartInputLinkerBase.h"
#include "Input/HeartInputTrip.h"
#include "Input/HeartInputTypes.h"
#include "Input/HeartInputActivation.h"

#include "UI/HeartWidgetInputLinkerRedirector.h"
#include "Move_To_UMG/HeartDragDropOperation.h"

#include "Components/Widget.h"

#include "HeartUMGInputLinker.generated.h"


/**
 * Binds to UWidgets and externalizes input functions. This is used to set the DefaultLinkerClass in a
 * FHeartWidgetInputBindingContainer property. It can be subclassed if necessary for additional custom features, but
 * that is not usually needed.
 */
UCLASS(BlueprintType)
class HEARTCORE_API UHeartWidgetInputLinker : public UHeartInputLinkerBase
{
	GENERATED_BODY()

protected:
	TOptional<FReply> TryCallbacks(const Heart::Input::FInputTrip& Trip, UWidget* Widget, const FHeartInputActivation& Activation);

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
	void BindToOnDragDetected(const Heart::Input::FInputTrip& Trip, const Heart::Input::FConditionalCallback_DDO& DragDropTrigger);
	void UnbindToOnDragDetected(const Heart::Input::FInputTrip& Trip);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetInputLinker")
	bool TriggerManualInput(UWidget* Widget, FName Key, const FHeartManualEvent& Activation);

private:
	// Input trips that begin a drag drop operation
	TMultiMap<Heart::Input::FInputTrip, Heart::Input::FConditionalCallback_DDO> DragDropTriggers;
};

namespace Heart::Input
{
	template <>
	struct TLinkerType<UWidget>
	{
		static constexpr bool Supported = true;

		using FValueType = UWidget*;
		using FDDOType = UDragDropOperation*;

		using FDescriptionDelegate = TSpecifiedDelegate<TDelegate<FText(const UWidget*)>>;
		using FConditionDelegate = TSpecifiedDelegate<TDelegate<bool(const UWidget*)>>;
		using FHandlerDelegate = TSpecifiedDelegate<TDelegate<FReply(UWidget*, const FHeartInputActivation&)>>;
		using FCreateDDODelegate = TSpecifiedDelegate<TDelegate<UHeartDragDropOperation*(UWidget*)>>;

		static UHeartWidgetInputLinker* FindLinker(const UWidget* Widget)
		{
			if (!ensure(IsValid(Widget))) return nullptr;

			for (auto&& Test = Widget; IsValid(Test); Test = Test->GetTypedOuter<UWidget>())
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
	};
}

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
	auto Operation = Heart::Input::LinkOnDragDetected<UWidget>(this, InGeometry, InMouseEvent);\
	if (Operation.IsSet())\
	{\
		OutOperation = Operation.GetValue();\
	}\
	else\
	{\
		Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);\
	}\
}\
\
bool type::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)\
{\
	bool Handled = Heart::Input::LinkOnDrop<UWidget>(this, InGeometry, InDragDropEvent, InOperation);\
	return Handled ? true : Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);\
}\
\
bool type::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)\
{\
	bool Handled = Heart::Input::LinkOnDragOver<UWidget>(this, InGeometry, InDragDropEvent, InOperation);\
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