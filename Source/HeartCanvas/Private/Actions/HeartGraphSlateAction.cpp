// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartGraphSlateAction.h"
#include "Input/HeartSlateReplyWrapper.h"
#include "Input/SlatePointerWrappers.h"
#include "ModelView/HeartActionHistory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphSlateAction)

FText UHeartGraphSlateAction::GetDescription(const UObject* Target) const
{
	if (auto&& SlateTarget = Cast<UHeartSlatePtr>(Target))
	{
		return GetDescription(SlateTarget);
	}
	return Super::GetDescription(Target);
}

bool UHeartGraphSlateAction::CanExecute(const UObject* Target) const
{
	if (auto&& SlateTarget = Cast<UHeartSlatePtr>(Target))
	{
		return CanExecuteOnSlatePtr(SlateTarget);
	}
	return false;
}

FHeartEvent UHeartGraphSlateAction::Execute(const Heart::Action::FArguments& Arguments) const
{
	auto&& Widget = Cast<UHeartSlatePtr>(Arguments.Target);

	if (!IsValid(Widget))
	{
		return FHeartEvent::Invalid;
	}

	return Heart::Action::History::Log(this, Arguments,
		[&](FBloodContainer& UndoData)
		{
			return ExecuteOnSlatePtr(Widget, Arguments.Activation, Arguments.Payload, UndoData);
		});
}

FText UHeartGraphSlateAction::GetDescription(const UHeartSlatePtr* SlatePtr) const
{
	return FText::GetEmpty();
}

bool UHeartGraphSlateAction::CanExecuteOnSlatePtr(const UHeartSlatePtr* SlatePtr) const
{
	return true;
}

FHeartEvent UHeartGraphSlateAction::ExecuteOnSlatePtr(UHeartSlatePtr* SlatePtr, const FHeartInputActivation& Activation,
													 UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (!IsValid(SlatePtr))
	{
		return FHeartEvent::Invalid;
	}

	if (auto&& Graph = Cast<UHeartSlateGraph>(SlatePtr))
    {
    	return Heart::Input::ReplyToHeartEvent(FHeartEvent::Handled,
    		ExecuteOnGraph(Graph, Activation, ContextObject, UndoData));
    }

    if (auto&& Node = Cast<UHeartSlateNode>(SlatePtr))
    {
    	return Heart::Input::ReplyToHeartEvent(FHeartEvent::Handled,
    		ExecuteOnNode(Node, Activation, ContextObject, UndoData));
    }

    if (auto&& Pin = Cast<UHeartSlatePin>(SlatePtr))
    {
    	return Heart::Input::ReplyToHeartEvent(FHeartEvent::Handled,
    		ExecuteOnPin(Pin, Activation, ContextObject, UndoData));
    }

	return FHeartEvent::Ignored;
}

FText UHeartGraphSlateAction_BlueprintBase::GetDescription(const UHeartSlatePtr* SlatePtr) const
{
	return BP_GetDescription(SlatePtr);
}

bool UHeartGraphSlateAction_BlueprintBase::CanExecuteOnSlatePtr(const UHeartSlatePtr* SlatePtr) const
{
	if (ensure(IsValid(SlatePtr)))
	{
		if (GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(ThisClass, BP_CanExecuteOnWidget)))
		{
			return BP_CanExecuteOnWidget(SlatePtr);
		}
	}
	return true;
}

FReply UHeartGraphSlateAction_BlueprintBase::ExecuteOnGraph(UHeartSlateGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (ensure(IsValid(Graph)))
	{
		return BP_ExecuteOnGraph(Graph, Activation, ContextObject, UndoData).NativeReply;
	}
	return FReply::Unhandled();
}

FReply UHeartGraphSlateAction_BlueprintBase::ExecuteOnNode(UHeartSlateNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (ensure(IsValid(Node)))
	{
		return BP_ExecuteOnNode(Node, Activation, ContextObject, UndoData).NativeReply;
	}
	return FReply::Unhandled();
}

FReply UHeartGraphSlateAction_BlueprintBase::ExecuteOnPin(UHeartSlatePin* Pin, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (ensure(IsValid(Pin)))
	{
		return BP_ExecuteOnPin(Pin, Activation, ContextObject, UndoData).NativeReply;
	}
	return FReply::Unhandled();
}

bool UHeartGraphSlateAction_BlueprintBase::CanUndo(const UObject* Target) const
{
	// A Blueprint action can undo if it has an implementation of BP_Undo
	return GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(ThisClass, BP_Undo));
}

bool UHeartGraphSlateAction_BlueprintBase::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	if (auto&& WidgetTarget = Cast<UHeartSlatePtr>(Target))
	{
		return BP_Undo(WidgetTarget, UndoData);
	}

	return false;
}