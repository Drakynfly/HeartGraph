// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartGraphCanvasAction.h"
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

bool UHeartGraphCanvasAction::Execute(const Heart::Action::FArguments& Arguments)
{
	if (auto&& Widget = Cast<UHeartGraphWidgetBase>(Arguments.Target))
	{
		const auto Handled = ExecuteOnWidget(Widget, Arguments.Activation, Arguments.Payload).IsEventHandled();

		if (Handled)
		{
			UHeartActionHistory::TryLogAction(this, Arguments);
		}

		return Handled;
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
	return FText::GetEmpty();
}

bool UHeartGraphCanvasAction::CanExecuteOnWidget(const UHeartGraphWidgetBase* Widget) const
{
	return true;
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

FEventReply UHeartGraphCanvasAction_BlueprintBase::ExecuteOnGraph(UHeartGraphCanvas* Graph, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (ensure(IsValid(Graph)))
	{
		return BP_ExecuteOnGraph(Graph, Activation, ContextObject);
	}
	return false;
}

FEventReply UHeartGraphCanvasAction_BlueprintBase::ExecuteOnNode(UHeartGraphCanvasNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (ensure(IsValid(Node)))
	{
		return BP_ExecuteOnNode(Node, Activation, ContextObject);
	}
	return false;
}

FEventReply UHeartGraphCanvasAction_BlueprintBase::ExecuteOnPin(UHeartGraphCanvasPin* Pin, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (ensure(IsValid(Pin)))
	{
		return BP_ExecuteOnPin(Pin, Activation, ContextObject);
	}
	return false;
}

bool UHeartGraphCanvasAction_BlueprintBase::CanUndo(UObject* Target) const
{
	// A Blueprint action can undo if it has an implementation of BP_Undo
	return GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(ThisClass, BP_Undo));
}

bool UHeartGraphCanvasAction_BlueprintBase::Undo(UObject* Target)
{
	if (auto&& WidgetTarget = Cast<UHeartGraphWidgetBase>(Target))
	{
		return BP_Undo(WidgetTarget);
	}

	return Super::Undo(Target);
}