// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartGraphCanvasAction.h"
#include "Input/HeartSlateReplyWrapper.h"
#include "ModelView/HeartActionHistory.h"
#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "UMG/HeartGraphCanvasPin.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphCanvasAction)

FText UHeartGraphCanvasAction::GetDescription(const UObject* Target) const
{
	if (auto&& Widget = Cast<UHeartGraphWidgetBase>(Target))
	{
		return GetDescription(Widget);
	}
	return Super::GetDescription(Target);
}

bool UHeartGraphCanvasAction::CanExecute(const UObject* Target) const
{
	if (auto&& Widget = Cast<UHeartGraphWidgetBase>(Target))
	{
		return CanExecuteOnWidget(Widget);
	}
	return false;
}

FHeartEvent UHeartGraphCanvasAction::Execute(const Heart::Action::FArguments& Arguments) const
{
	auto&& Widget = Cast<UHeartGraphWidgetBase>(Arguments.Target);

	if (!IsValid(Widget))
	{
		return FHeartEvent::Invalid;
	}

	return Heart::Action::History::Log(this, Arguments,
		[&](FBloodContainer& UndoData)
		{
			return ExecuteOnWidget(Widget, Arguments.Activation, Arguments.Payload, UndoData);
		});
}

FText UHeartGraphCanvasAction::GetDescription(const UHeartGraphWidgetBase* Widget) const
{
	return FText::GetEmpty();
}

bool UHeartGraphCanvasAction::CanExecuteOnWidget(const UHeartGraphWidgetBase* Widget) const
{
	return true;
}

FHeartEvent UHeartGraphCanvasAction::ExecuteOnWidget(UHeartGraphWidgetBase* Widget, const FHeartInputActivation& Activation,
													 UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (!IsValid(Widget))
	{
		return FHeartEvent::Invalid;
	}

	if (auto&& Graph = Cast<UHeartGraphCanvas>(Widget))
    {
    	return Heart::Input::ReplyToHeartEvent(FHeartEvent::Handled,
    		ExecuteOnGraph(Graph, Activation, ContextObject, UndoData));
    }

    if (auto&& Node = Cast<UHeartGraphCanvasNode>(Widget))
    {
    	return Heart::Input::ReplyToHeartEvent(FHeartEvent::Handled,
    		ExecuteOnNode(Node, Activation, ContextObject, UndoData));
    }

    if (auto&& Pin = Cast<UHeartGraphCanvasPin>(Widget))
    {
    	return Heart::Input::ReplyToHeartEvent(FHeartEvent::Handled,
    		ExecuteOnPin(Pin, Activation, ContextObject, UndoData));
    }

	return FHeartEvent::Ignored;
}

FText UHeartGraphCanvasAction_BlueprintBase::GetDescription(const UHeartGraphWidgetBase* Widget) const
{
	return BP_GetDescription(Widget);
}

bool UHeartGraphCanvasAction_BlueprintBase::CanExecuteOnWidget(const UHeartGraphWidgetBase* Widget) const
{
	if (ensure(IsValid(Widget)))
	{
		if (GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(ThisClass, BP_CanExecuteOnWidget)))
		{
			return BP_CanExecuteOnWidget(Widget);
		}
	}
	return true;
}

FReply UHeartGraphCanvasAction_BlueprintBase::ExecuteOnGraph(UHeartGraphCanvas* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (ensure(IsValid(Graph)))
	{
		return BP_ExecuteOnGraph(Graph, Activation, ContextObject, UndoData).NativeReply;
	}
	return FReply::Unhandled();
}

FReply UHeartGraphCanvasAction_BlueprintBase::ExecuteOnNode(UHeartGraphCanvasNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (ensure(IsValid(Node)))
	{
		return BP_ExecuteOnNode(Node, Activation, ContextObject, UndoData).NativeReply;
	}
	return FReply::Unhandled();
}

FReply UHeartGraphCanvasAction_BlueprintBase::ExecuteOnPin(UHeartGraphCanvasPin* Pin, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (ensure(IsValid(Pin)))
	{
		return BP_ExecuteOnPin(Pin, Activation, ContextObject, UndoData).NativeReply;
	}
	return FReply::Unhandled();
}

bool UHeartGraphCanvasAction_BlueprintBase::CanUndo(const UObject* Target) const
{
	// A Blueprint action can undo if it has an implementation of BP_Undo
	return GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(ThisClass, BP_Undo));
}

bool UHeartGraphCanvasAction_BlueprintBase::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	if (auto&& WidgetTarget = Cast<UHeartGraphWidgetBase>(Target))
	{
		return BP_Undo(WidgetTarget, UndoData);
	}

	return false;
}