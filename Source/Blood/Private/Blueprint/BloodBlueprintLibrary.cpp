// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "BloodBlueprintLibrary.h"
#include "BloodContainer.h"
#include "BloodFProperty.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BloodBlueprintLibrary)

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

void UBloodBlueprintLibrary::AddBloodValue(FBloodContainer& Container, const FName Name, const FBloodValue& Value)
{
	Container.AddBloodValue(Name, Value);
}

void UBloodBlueprintLibrary::Remove(FBloodContainer& Container, const FName Name)
{
	Container.Remove(Name);
}

void UBloodBlueprintLibrary::Clear(FBloodContainer& Container)
{
	Container.Clear();
}

void UBloodBlueprintLibrary::GetBloodValue(const FBloodContainer& Container, const FName Name, FBloodValue& Value)
{
	if (TOptional<FBloodValue> Option = Container.GetBloodValue(Name);
		Option.IsSet())
	{
		Value = Option.GetValue();
	}
	else
	{
		Value = FBloodValue();
	}
}

bool UBloodBlueprintLibrary::FindBloodValue(const FBloodContainer& Container, const FName Name, FBloodValue& Value)
{
	GetBloodValue(Container, Name, Value);
	return Value.IsValid();
}

bool UBloodBlueprintLibrary::Contains(const FBloodContainer& Container, const FName Name)
{
	return Container.Contains(Name);
}

int32 UBloodBlueprintLibrary::Num(const FBloodContainer& Container)
{
	return Container.Num();
}

bool UBloodBlueprintLibrary::IsEmpty(const FBloodContainer& Container)
{
	return Container.IsEmpty();
}

DEFINE_FUNCTION(UBloodBlueprintLibrary::execReadProperty)
{
	P_GET_OBJECT(UObject, Object);
	P_GET_PROPERTY(FNameProperty, PropertyName);

	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn<FProperty>(nullptr);

	const FProperty* ValueProp = Stack.MostRecentProperty;

	P_FINISH;

	if (!IsValid(Object))
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
			LOCTEXT("execReadProperty_MakeInvalidObjectWarning", "Failed to resolve the Object for Read Property")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		return;
	}

	if (!ValueProp)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
			LOCTEXT("execReadProperty_MakeInvalidValueWarning", "Failed to resolve the output Value for Read Property")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		return;
	}

	P_NATIVE_BEGIN;
	if (const FProperty* Property = Object->GetClass()->FindPropertyByName(PropertyName))
	{
		const FBloodValue BloodValue = Blood::ReadFromFProperty(Object, Property);
		ValueProp->CopyCompleteValueToScriptVM(Object, BloodValue.GetMemory());
	}
	P_NATIVE_END;
}

DEFINE_FUNCTION(UBloodBlueprintLibrary::execPropertyToBlood)
{
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn<FProperty>(nullptr);

	const FProperty* ValueProp = Stack.MostRecentProperty;
	const uint8* ValuePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (!ValueProp || !ValuePtr)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
			LOCTEXT("execPropertyToBlood_MakeInvalidValueWarning", "Failed to resolve the Property for Property to Blood")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);

		P_NATIVE_BEGIN;
		static_cast<FBloodValue*>(RESULT_PARAM)->Reset();
		P_NATIVE_END;
	}
	else
	{
		P_NATIVE_BEGIN;
		*static_cast<FBloodValue*>(RESULT_PARAM) = Blood::ReadFromFProperty(ValueProp, ValuePtr);
		P_NATIVE_END;
	}
}

DEFINE_FUNCTION(UBloodBlueprintLibrary::execBloodToProperty)
{
	P_GET_STRUCT_REF(FBloodValue, Value)

	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn<FProperty>(nullptr);

	const FProperty* ValueProp = Stack.MostRecentProperty;
	uint8* ValuePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (!ValueProp || !ValuePtr || !Value.IsValid())
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
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
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn<FProperty>(nullptr);

	const FProperty* ValueProp = Stack.MostRecentProperty;
	const uint8* ValuePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (!ValueProp || !ValuePtr)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
			LOCTEXT("execAssignPropertyToBlood_MakeInvalidValueWarning", "Failed to resolve the Property for Assign Property to Blood")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);

		P_NATIVE_BEGIN;
		Value.Reset();
		*static_cast<bool*>(RESULT_PARAM) = false;
		P_NATIVE_END;
	}
	else
	{
		P_NATIVE_BEGIN;
		Value = Blood::ReadFromFProperty(ValueProp, ValuePtr);
		*static_cast<bool*>(RESULT_PARAM) = Value.IsValid();
		P_NATIVE_END;
	}
}

DEFINE_FUNCTION(UBloodBlueprintLibrary::execAssignBloodToProperty)
{
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn<FProperty>(nullptr);

	const FProperty* ValueProp = Stack.MostRecentProperty;
	uint8* ValuePtr = Stack.MostRecentPropertyAddress;

	P_GET_STRUCT_REF(FBloodValue, Value)

	P_FINISH;

	if (!ValueProp || !ValuePtr)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
			LOCTEXT("execAssignBloodToProperty_MakeInvalidValueWarning", "Failed to resolve the Property for Assign Blood to Property")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else
	{
		P_NATIVE_BEGIN;
		*static_cast<bool*>(RESULT_PARAM) = Blood::WriteToFProperty(ValueProp, ValuePtr, Value);
		P_NATIVE_END;
	}
}

#undef LOCTEXT_NAMESPACE