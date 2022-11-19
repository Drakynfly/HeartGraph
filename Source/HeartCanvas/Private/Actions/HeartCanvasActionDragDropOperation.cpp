// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasActionDragDropOperation.h"
#include "Actions/HeartGraphCanvasAction.h"
#include "Components/Widget.h"

void UHeartCanvasActionDragDropOperation::Drop_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Drop_Implementation(PointerEvent);

	if (auto&& Action = Cast<UHeartGraphCanvasAction>(Payload))
	{
		FHeartInputActivation Activation;
		Activation.ActivationValue = 0;
		Action->Execute(GetHoveredWidget(), Activation);
	}
}
