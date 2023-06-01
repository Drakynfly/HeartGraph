// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "BloodBlueprintLibrary.h"

#define LOCTEXT_NAMESPACE "BloodBlueprintLibrary"

void UBloodBlueprintLibrary::ReadProperty(const UObject*, FName Property, uint8& Value)
{
	// Does not function. Stub for execPropertyToBlood
	check(0);
}

FBloodValue UBloodBlueprintLibrary::PropertyToBlood(const uint8&)
{
	// Does not function. Stub for execPropertyToBlood
	check(0);
	return FBloodValue();
}

void UBloodBlueprintLibrary::BloodToProperty(const FBloodValue&, uint8&)
{
	// Does not function. Stub for execBloodToProperty
	check(0);
}

bool UBloodBlueprintLibrary::AssignPropertyToBlood(FBloodValue&, const uint8&)
{
	// Does not function. Stub for execAssignPropertyToBlood
	check(0);
	return false;
}

bool UBloodBlueprintLibrary::AssignBloodToProperty(uint8&, const FBloodValue&)
{
	// Does not function. Stub for execBloodToProperty
	check(0);
	return false;
}

DEFINE_FUNCTION(UBloodBlueprintLibrary::execReadProperty)
{
	P_GET_OBJECT(UObject, Object);
	P_GET_PROPERTY(FNameProperty, PropertyName);

	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn<FProperty>(nullptr);

	const FProperty* ValueProp = Stack.MostRecentProperty;

	P_FINISH;

	if (!IsValid(Object))
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::NonFatalError,
			LOCTEXT("execReadProperty_MakeInvalidObjectWarning", "Failed to resolve the Object for Read Property")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		return;
	}

	if (!ValueProp)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::NonFatalError,
			LOCTEXT("execReadProperty_MakeInvalidValueWarning", "Failed to resolve the output Value for Read Property")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		return;
	}

	P_NATIVE_BEGIN;
	if (const FProperty* Property = Object->GetClass()->FindPropertyByName(PropertyName))
	{
		const FBloodValue BloodValue = Blood::ReadFromFProperty(Object, Property);
		ValueProp->CopyCompleteValueToScriptVM(Object, BloodValue.Data.GetMemory());
	}
	P_NATIVE_END;
}

DEFINE_FUNCTION(UBloodBlueprintLibrary::execPropertyToBlood)
{
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn<FProperty>(nullptr);

	const FProperty* ValueProp = Stack.MostRecentProperty;
	const uint8* ValuePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (!ValueProp || !ValuePtr)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::NonFatalError,
			LOCTEXT("execPropertyToBlood_MakeInvalidValueWarning", "Failed to resolve the Property for Property to Blood")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);

		P_NATIVE_BEGIN;
		(*(FBloodValue*)RESULT_PARAM).Data.Reset();
		P_NATIVE_END;
	}
	else
	{
		P_NATIVE_BEGIN;
		(*(FBloodValue*)RESULT_PARAM) = Blood::ReadFromFProperty(ValueProp, ValuePtr);
		P_NATIVE_END;
	}
}

DEFINE_FUNCTION(UBloodBlueprintLibrary::execBloodToProperty)
{
	P_GET_STRUCT_REF(FBloodValue, Value)

	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn<FProperty>(nullptr);

	const FProperty* ValueProp = Stack.MostRecentProperty;
	uint8* ValuePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (!ValueProp || !ValuePtr)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::NonFatalError,
			LOCTEXT("execBloodToProperty_MakeInvalidValueWarning", "Failed to resolve the Property for Blood to Property")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else
	{
		P_NATIVE_BEGIN;
		Blood::WriteToFProperty(ValueProp, ValuePtr, Value);
		P_NATIVE_END;
	}
}

DEFINE_FUNCTION(UBloodBlueprintLibrary::execAssignPropertyToBlood)
{
	P_GET_STRUCT_REF(FBloodValue, Value)

	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn<FProperty>(nullptr);

	const FProperty* ValueProp = Stack.MostRecentProperty;
	const uint8* ValuePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (!ValueProp || !ValuePtr)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::NonFatalError,
			LOCTEXT("execAssignPropertyToBlood_MakeInvalidValueWarning", "Failed to resolve the Property for Assign Property to Blood")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);

		P_NATIVE_BEGIN;
		Value.Data.Reset();
		*(bool*)RESULT_PARAM = false;
		P_NATIVE_END;
	}
	else
	{
		P_NATIVE_BEGIN;
		Value = Blood::ReadFromFProperty(ValueProp, ValuePtr);
		*(bool*)RESULT_PARAM = Value.Data.IsValid();
		P_NATIVE_END;
	}
}

DEFINE_FUNCTION(UBloodBlueprintLibrary::execAssignBloodToProperty)
{
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn<FProperty>(nullptr);

	const FProperty* ValueProp = Stack.MostRecentProperty;
	uint8* ValuePtr = Stack.MostRecentPropertyAddress;

	P_GET_STRUCT_REF(FBloodValue, Value)

	P_FINISH;

	if (!ValueProp || !ValuePtr)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::NonFatalError,
			LOCTEXT("execAssignBloodToProperty_MakeInvalidValueWarning", "Failed to resolve the Property for Assign Blood to Property")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else
	{
		P_NATIVE_BEGIN;
		*(bool*)RESULT_PARAM = Blood::WriteToFProperty(ValueProp, ValuePtr, Value);
		P_NATIVE_END;
	}
}

#undef LOCTEXT_NAMESPACE