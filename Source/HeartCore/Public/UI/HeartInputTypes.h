// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

class UWidget;
class UHeartDragDropOperation;

DECLARE_DELEGATE_RetVal_OneParam(FText, FHeartWidgetInputDescription, const UWidget* /** Widget */);

DECLARE_DELEGATE_RetVal_OneParam(bool, FHeartWidgetInputCondition, const UWidget* /** Widget */);

DECLARE_DELEGATE_RetVal_TwoParams(
	FReply, FHeartWidgetLinkedEventCallback, UWidget* /** Widget */, const struct FHeartInputActivation& /** Activation */);

DECLARE_DELEGATE_RetVal_OneParam(
	UHeartDragDropOperation*, FHeartWidgetLinkedDragDropTriggerCreate, UWidget* /** Widget */);

namespace Heart::Input
{
	enum EHeartInputLayer
	{
		None, // Blank layer. Do not use.
		Event, // Default layer. Events may return FReply::Handled
		Listener, // Interception layer. Listeners always return FReply::Unhandled
	};

	struct FConditionalInputBase
	{
		// Callback to retrieve a text description of the action
		FHeartWidgetInputDescription Description;

		// Callback to determine if the context of the trigger is valid for executing the action
		FHeartWidgetInputCondition Condition;

		// Input layers determine the priority of callbacks, and whether they Handle the input callstack
		EHeartInputLayer Layer = None;

		friend bool operator<(const FConditionalInputBase& A, const FConditionalInputBase& B)
		{
			// Sort in reverse. Higher layers should be ordered first, lower layers after.
			return A.Layer > B.Layer;
		}
	};

	struct FConditionalInputCallback : FConditionalInputBase
	{
		// Callback to execute the event
		FHeartWidgetLinkedEventCallback Callback;
	};

	struct FConditionalDragDropTrigger : FConditionalInputBase
	{
		// Callback to begin a DDO
		FHeartWidgetLinkedDragDropTriggerCreate Callback;
	};
}
