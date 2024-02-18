// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartNamedFunctionEvent.h"

#include "Components/Widget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartNamedFunctionEvent)

FReply UHeartNamedFunctionEvent::TriggerEvent(UWidget* Widget, const FHeartInputActivation& Trip) const
{
	if (IsValid(Widget))
	{
		const FName FunctionFName(*FunctionName);

		UFunction* const Func = Widget->FindFunction(FunctionFName);
		if (Func && Func->ParmsSize > 0)
		{
			// Passed in a valid function, but one that takes parameters
			UE_LOG(LogBlueprintUserMessages, Warning, TEXT("NamedFunctionEvent passed a function (%s) that expects parameters."), *FunctionName);
			return FReply::Unhandled();
		}

		void* Parameters;
		Widget->ProcessEvent(Func, &Parameters);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}