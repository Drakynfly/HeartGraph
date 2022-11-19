// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartGraphCanvasActionBinding.h"
#include "Actions/HeartGraphCanvasAction.h"
#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "UMG/HeartGraphCanvasPin.h"

FHeartWidgetLinkedEvent UHeartGraphCanvasActionBinding::CreateEvent() const
{
	return {FHeartWidgetLinkedEventCallback::CreateLambda([this](UWidget* Widget, const FHeartInputActivation& Activation)
	{
		if (Widget->IsA<UHeartGraphCanvas>() ||
			Widget->IsA<UHeartGraphCanvasNode>() ||
			Widget->IsA<UHeartGraphCanvasPin>())
		{
			if (UHeartGraphActionBase::QuickExecuteGraphAction(ActionClass, Widget, Activation))
			{
				return FReply::Handled();
			}
		}

		return FReply::Unhandled();
	}), Heart::Input::Event };
}

FHeartWidgetLinkedListener UHeartGraphCanvasActionListener::CreateListener() const
{
	return {FHeartWidgetLinkedListenerCallback::CreateLambda([this](UWidget* Widget, const FHeartInputActivation& Activation)
	{
		if (Widget->IsA<UHeartGraphCanvas>() ||
			Widget->IsA<UHeartGraphCanvasNode>() ||
			Widget->IsA<UHeartGraphCanvasPin>())
		{
			UHeartGraphActionBase::QuickExecuteGraphAction(ActionClass, Widget, Activation);
		}
	})};
}
