// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartCanvasInputHandler_DDO_Action.h"
#include "Actions/HeartGraphCanvasAction.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "General/HeartContextObject.h"
#include "Input/SlatePointerWrappers.h"
#include "UMG/HeartGraphWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasInputHandler_DDO_Action)

bool UHeartCanvasActionDragDropOperation::SetupDragDropOperation()
{
	return IsValid(Action);
}

void UHeartCanvasActionDragDropOperation::Drop_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Drop_Implementation(PointerEvent);

	if (ensure(IsValid(Action)))
	{
		Heart::Action::Execute(Action, GetHoveredWidget(), PointerEvent, Payload);
	}
}

namespace Heart::Canvas
{
	FReply FSlateActionDragDropOperation::OnHoverWidget(const TSharedRef<SWidget>& Widget)
	{
		LastHovered = Widget;
		return FNativeDragDropOperation::OnHoverWidget(Widget);
	}

	bool FSlateActionDragDropOperation::SetupDragDropOperation()
	{
		return IsValid(Action);
	}

	void FSlateActionDragDropOperation::OnDrop(const bool bDropWasHandled, const FPointerEvent& MouseEvent)
	{
		FNativeDragDropOperation::OnDrop(bDropWasHandled, MouseEvent);

		if (ensure(IsValid(Action)) && LastHovered.IsValid())
		{
			Action::Execute(Action, UHeartSlatePtr::Wrap(LastHovered.ToSharedRef()), MouseEvent, nullptr); // @todo Payload for slate DDOs
		}
	}
}


bool UHeartCanvasInputHandler_DDO_Action::PassCondition(const UObject* TestTarget) const
{
	bool Failed = !Super::PassCondition(TestTarget);

	if (IsValid(ActionClass))
	{
		Failed |= !Heart::Action::CanExecute(ActionClass, TestTarget);
	}

	return !Failed;
}

FHeartEvent UHeartCanvasInputHandler_DDO_Action::OnTriggered(UObject* Target, const FHeartInputActivation& Activation) const
{
	// Case 1: Target is a UMG widget
	if (UWidget* Widget = Cast<UWidget>(Target))
	{
		auto&& NewDDO = NewObject<UHeartCanvasActionDragDropOperation>(GetTransientPackage());

		NewDDO->SummonedBy = Widget;

		if (IsValid(VisualClass))
		{
			auto&& NewVisual = CreateWidget(Widget, VisualClass);

			// If both the widget and the visual want a context object pass it between them
			if (Widget->Implements<UHeartContextObject>() &&
				NewVisual->Implements<UHeartContextObject>())
			{
				auto&& Context = IHeartContextObject::Execute_GetContextObject(Widget);
				IHeartContextObject::Execute_SetContextObject(NewVisual, Context);
			}

			NewDDO->DefaultDragVisual = NewVisual;
			NewDDO->Pivot = Pivot;
			NewDDO->Offset = Offset;
		}

		if (Widget->Implements<UHeartContextObject>())
		{
			NewDDO->Payload = IHeartContextObject::Execute_GetContextObject(Widget);
		}

		NewDDO->Action = ActionClass;

		return FHeartEvent::Handled.Detail<FHeartDeferredEvent>(NewDDO);
	}

	// Case 2: Target is a slate widget
	if (UHeartSlatePtr* SlatePtr = Cast<UHeartSlatePtr>(Target))
	{
		auto&& Operation = MakeShared<Heart::Canvas::FSlateActionDragDropOperation>();

		Operation->SummonedBy = SlatePtr->GetWidget();

		Operation->Action = ActionClass;

		UHeartSlateDragDropOperation* SlateDragDropOperation = NewObject<UHeartSlateDragDropOperation>();
		SlateDragDropOperation->SlatePointer = Operation;
		return FHeartEvent::Handled.Detail<FHeartDeferredEvent>(SlateDragDropOperation);
	}

	// Target is not a type this handler accepts
	return FHeartEvent::Failed;
}