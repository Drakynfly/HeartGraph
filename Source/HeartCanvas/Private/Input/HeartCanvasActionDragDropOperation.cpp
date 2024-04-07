// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartCanvasInputHandler_DDO_Action.h"
#include "HeartCanvasLog.h"
#include "Actions/HeartGraphCanvasAction.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "General/HeartContextObject.h"
#include "UMG/HeartGraphWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasInputHandler_DDO_Action)

bool UHeartCanvasActionDragDropOperation::SetupDragDropOperation()
{
	return IsValid(Action);
}

void UHeartCanvasActionDragDropOperation::Drop_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Drop_Implementation(PointerEvent);

	if (IsValid(Action))
	{
		Heart::Action::Execute(Action, GetHoveredWidget(), PointerEvent, Payload);
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
	UWidget* Widget = Cast<UWidget>(Target);

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

	if (!NewDDO->SetupDragDropOperation())
	{
		UE_LOG(LogHeartCanvas, Warning, TEXT("Created DDO (%s) unnecessarily, figure out why"), *NewDDO->GetClass()->GetName())
		return FHeartEvent::Failed;
	}

	return FHeartEvent::Handled.Detail<FHeartDeferredEvent>(NewDDO);
}