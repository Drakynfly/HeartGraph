// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartGraphCanvasActionBinding.h"
#include "Actions/HeartGraphCanvasAction.h"
#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "UMG/HeartGraphCanvasPin.h"

FReply UHeartGraphCanvasActionBinding::TriggerEvent(UWidget* Widget, const FHeartInputActivation& Trip) const
{
	if (Widget->IsA<UHeartGraphCanvas>() ||
			Widget->IsA<UHeartGraphCanvasNode>() ||
			Widget->IsA<UHeartGraphCanvasPin>())
	{
		if (UHeartGraphActionBase::QuickExecuteGraphAction(ActionClass, Widget, Trip))
		{
			FReply Reply = FReply::Handled();

			if (CaptureMouse)
			{
				const TSharedPtr<SWidget> CapturingSlateWidget = Widget->GetCachedWidget();
				if (CapturingSlateWidget.IsValid())
				{
					Reply.CaptureMouse(CapturingSlateWidget.ToSharedRef());
				}
			}

			return Reply;
		}
	}

	return FReply::Unhandled();
}