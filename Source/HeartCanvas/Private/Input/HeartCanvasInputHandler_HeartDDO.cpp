// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartCanvasInputHandler_HeartDDO.h"
#include "HeartCanvasLog.h"
#include "Blueprint/UserWidget.h"
#include "General/HeartContextObject.h"
#include "Input/HeartDragDropOperation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasInputHandler_HeartDDO)

bool UHeartCanvasInputHandler_HeartDDO::PassCondition(const UObject* TestTarget) const
{
	bool Failed = !Super::PassCondition(TestTarget);

	if (auto&& TestWidget = Cast<UWidget>(TestTarget))
	{
		Failed |= !GetDefault<UHeartDragDropOperation>(OperationClass)->CanRunOnWidget(TestWidget);
	}
	else
	{
		Failed = true;
	}

	return !Failed;
}

FHeartEvent UHeartCanvasInputHandler_HeartDDO::OnTriggered(UObject* Target, const FHeartInputActivation& Activation) const
{
	UWidget* Widget = Cast<UWidget>(Target);

	auto&& NewDDO = NewObject<UHeartDragDropOperation>(GetTransientPackage(), OperationClass);

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

	if (!NewDDO->SetupDragDropOperation())
	{
		UE_LOG(LogHeartCanvas, Warning, TEXT("Created DDO (%s) unnecessarily, figure out why"), *NewDDO->GetClass()->GetName())
		return FHeartEvent::Invalid;
	}

	return FHeartEvent::Handled.Detail<FHeartDeferredEvent>(NewDDO);
}