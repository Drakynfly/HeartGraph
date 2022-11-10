// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/NamedFunctionEvent.h"

FHeartWidgetLinkedInput UNamedFunctionEvent::CreateDelegate() const
{
	return FHeartWidgetLinkedInput::CreateLambda([this](UWidget* Widget, const FHeartInputActivation& Activation)
	{
		if (Widget)
		{
			const FName FunctionFName(*FunctionName);

			UFunction* const Func = Widget->FindFunction(FunctionFName);
			if (Func && (Func->ParmsSize > 0))
			{
				// User passed in a valid function, but one that takes parameters
				// FTimerDynamicDelegate expects zero parameters and will choke on execution if it tries
				// to execute a mismatched function
				UE_LOG(LogBlueprintUserMessages, Warning, TEXT("AddNamedFunction passed a function (%s) that expects parameters."), *FunctionName);
				return FReply::Unhandled();
			}

			void* Parameters;
			Widget->ProcessEvent(Func, &Parameters);
			return FReply::Handled();
		}

		return FReply::Unhandled();
	});
}
