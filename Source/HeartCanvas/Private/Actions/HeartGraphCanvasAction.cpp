// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartGraphCanvasAction.h"
#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "UMG/HeartGraphCanvasPin.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphCanvasAction)

FText UHeartGraphCanvasAction::GetDescription(const UObject* Object) const
{
	if (auto&& Widget = Cast<UHeartGraphWidgetBase>(Object))
	{
		return GetDescription(Widget);
	}
	return Super::GetDescription(Object);
}

bool UHeartGraphCanvasAction::CanExecute(const UObject* Object) const
{
	if (auto&& Widget = Cast<UHeartGraphWidgetBase>(Object))
	{
		return CanExecuteOnWidget(Widget);
	}
	return false;
}

bool UHeartGraphCanvasAction::Execute(UObject* Object, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (auto&& Widget = Cast<UHeartGraphWidgetBase>(Object))
	{
		return ExecuteOnWidget(Widget, Activation, ContextObject).IsEventHandled();
	}

	return false;
}

FReply UHeartGraphCanvasAction::ExecuteOnWidget(UHeartGraphWidgetBase* Widget, const FHeartInputActivation& Activation,
	UObject* ContextObject)
{
	if (auto&& Graph = Cast<UHeartGraphCanvas>(Widget))
    {
    	return ExecuteOnGraph(Graph, Activation, ContextObject).NativeReply;
    }

    if (auto&& Node = Cast<UHeartGraphCanvasNode>(Widget))
    {
    	return ExecuteOnNode(Node, Activation, ContextObject).NativeReply;
    }

    if (auto&& Pin = Cast<UHeartGraphCanvasPin>(Widget))
    {
    	return ExecuteOnPin(Pin, Activation, ContextObject).NativeReply;
    }

	return FReply::Unhandled();
}

FText UHeartGraphCanvasAction::GetDescription(const UHeartGraphWidgetBase* Widget) const
{
	return FText();
}

bool UHeartGraphCanvasAction::CanExecuteOnWidget(const UHeartGraphWidgetBase* Widget) const
{
	return true;
}

FText UHeartGraphCanvasActionBlueprintBase::GetDescription(const UHeartGraphWidgetBase* Widget) const
{
	return BP_GetDescription(Widget);
}

bool UHeartGraphCanvasActionBlueprintBase::CanExecuteOnWidget(const UHeartGraphWidgetBase* Widget) const
{
	if (ensure(IsValid(Widget)))
	{
		return BP_CanExecuteOnWidget(Widget);
	}
	return false;
}

FEventReply UHeartGraphCanvasActionBlueprintBase::ExecuteOnGraph(UHeartGraphCanvas* Graph, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (ensure(IsValid(Graph)))
	{
		return BP_ExecuteOnGraph(Graph, Activation, ContextObject);
	}
	return false;
}

FEventReply UHeartGraphCanvasActionBlueprintBase::ExecuteOnNode(UHeartGraphCanvasNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (ensure(IsValid(Node)))
	{
		return BP_ExecuteOnNode(Node, Activation, ContextObject);
	}
	return false;
}

FEventReply UHeartGraphCanvasActionBlueprintBase::ExecuteOnPin(UHeartGraphCanvasPin* Pin, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (ensure(IsValid(Pin)))
	{
		return BP_ExecuteOnPin(Pin, Activation, ContextObject);
	}
	return false;
}