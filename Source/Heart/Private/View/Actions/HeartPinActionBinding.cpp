// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "View/Actions/HeartPinActionBinding.h"
#include "Model/HeartGraph.h"
#include "ModelView/Actions/HeartGraphActionBase.h"
#include "View/HeartGraphCanvas.h"
#include "View/HeartGraphCanvasNode.h"
#include "View/HeartGraphCanvasPin.h"

FHeartWidgetLinkedInput UHeartGraphActionBinding::CreateDelegate() const
{
	return FHeartWidgetLinkedInput::CreateLambda([this](UWidget* Widget, const FHeartInputActivation& Activation)
	{
		if (UObject* RelevantObject = GetRelevantObject(Widget))
		{
			if (UHeartGraphActionBase::QuickExecuteGraphAction(ActionClass, RelevantObject, Activation))
			{
				return FReply::Handled();
			}
		}

		return FReply::Unhandled();
	});
}

UObject* UHeartGraphActionBinding::GetRelevantObject(UWidget* Widget) const
{
	return nullptr;
}

UObject* UAddGraphActionBinding::GetRelevantObject(UWidget* Widget) const
{
	if (const UHeartGraphCanvas* Canvas = Cast<UHeartGraphCanvas>(Widget))
	{
		return Canvas->GetGraph();
	}

	return nullptr;
}

UObject* UAddNodeActionBinding::GetRelevantObject(UWidget* Widget) const
{
	if (const UHeartGraphCanvasNode* CanvasNode = Cast<UHeartGraphCanvasNode>(Widget))
	{
		return CanvasNode->GetNode();
	}

	return nullptr;
}

UObject* UAddPinActionBinding::GetRelevantObject(UWidget* Widget) const
{
	if (const UHeartGraphCanvasPin* CanvasPin = Cast<UHeartGraphCanvasPin>(Widget))
	{
		return CanvasPin->GetPin();
	}

	return nullptr;
}

UObject* UAddCanvasGraphActionBinding::GetRelevantObject(UWidget* Widget) const
{
	return Cast<UHeartGraphCanvas>(Widget);
}

UObject* UAddCanvasNodeActionBinding::GetRelevantObject(UWidget* Widget) const
{
	return Cast<UHeartGraphCanvasNode>(Widget);
}

UObject* UAddCanvasPinActionBinding::GetRelevantObject(UWidget* Widget) const
{
	return Cast<UHeartGraphCanvasPin>(Widget);
}
