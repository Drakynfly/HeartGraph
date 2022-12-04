// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartDragDropOperation.h"
#include "HeartWidgetInputCondition.h"

DECLARE_DELEGATE_RetVal_TwoParams(
	FReply, FHeartWidgetLinkedEventCallback, UWidget* /** Widget */, const struct FHeartInputActivation& /** Activation */);

DECLARE_DELEGATE_RetVal_OneParam(
	UHeartDragDropOperation*, FHeartWidgetLinkedDragDropTriggerCreate, UWidget* /** Widget */);

namespace Heart::Input
{
	enum EHeartInputLayer
	{
		None, // Blank layer. Do not use.
		Event, // Default layer. Events cause FReply::Handled to be returned
		Listener, // Interception layer. Events cause FReply::Unhandled to be returned
	};

	struct FConditionalInputBase
	{
		FHeartWidgetInputCondition Condition;
		EHeartInputLayer Layer = None;

		friend bool operator<(const FConditionalInputBase& A, const FConditionalInputBase& B)
		{
			// Sort in reverse. Higher layers should be ordered first, lower layers after.
			return A.Layer > B.Layer;
		}
	};

	struct FConditionalInputCallback : FConditionalInputBase
	{
		FHeartWidgetLinkedEventCallback Callback;
	};

	struct FConditionalDragDropTrigger : FConditionalInputBase
	{
		FHeartWidgetLinkedDragDropTriggerCreate Callback;
	};
}
