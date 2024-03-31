// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "BloodValue.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(BloodValueTest,
								 "Blood.BloodValueTest",
								 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool BloodValueTest::RunTest(const FString& Parameters)
{
	static const TArray<bool> TestBoolArray = { false, false, true, false, true, true, true, false };
	static const TArray<FVector> TestVectorArray = { FVector(1, 2, 3), FVector(4, 1, 3), FVector(100., 600., 40000.), FVector(0, 1, 0) };
	static const TArray<FVector3f> TestVector3fArray = { FVector3f(1, 2, 3), FVector3f(4, 1, 3), FVector3f(100., 600., 40000.), FVector3f(0, 1, 0) };
	static const TSet<TSubclassOf<UObject>> TestClassSet = { AActor::StaticClass(), APlayerController::StaticClass(), UClass::StaticClass() };
	static const TMap<int32, FString> TestMap1 = {
		{ 1, "Hello, "}, { 2, "World!"} };

	static const TMap<EPropertyBagPropertyType, FVector3f> TestMap2 = {
		{ EPropertyBagPropertyType::Bool, FVector3f(1, 2, 3)},
		{ EPropertyBagPropertyType::Float, FVector3f(4, 1, 3)},
		{ EPropertyBagPropertyType::Struct, FVector3f(100., 600., 40000.)} };

	const FBloodValue Val_BoolTrue(true);
	const FBloodValue Val_BoolFalse(false);
	const FBloodValue Val_Byte = Blood::ToBloodValue<uint8>(137);
	const FBloodValue Val_Enum(EPropertyBagPropertyType::Enum);
	const FBloodValue Val_I32 = Blood::ToBloodValue<int32>(10259);
	const FBloodValue Val_I64 = Blood::ToBloodValue<int64>(8689101304);

	const FBloodValue Val_Name(FName("Hello, World!"));
	const FBloodValue Val_String(FString("Hello, World!"));
	const FBloodValue Val_Text(FText::FromString("Hello, World!"));

	const FBloodValue Val_BoolArray(TestBoolArray);
	const FBloodValue Val_VectorArray(TestVectorArray);
	const FBloodValue Val_Vector3fArray(TestVector3fArray);
	const FBloodValue Val_ClassSet(TestClassSet);

	const FBloodValue Val_I32StringMap = Blood::ToBloodValue(TestMap1);
	const FBloodValue Val_EnumStructMap = Blood::ToBloodValue(TestMap2);

	TestTrue("Blood is Boolean", Val_BoolTrue.Is<bool>());
	TestTrue("Blood is Byte", Val_Byte.Is<uint8>());
	TestTrue("Blood is Enum", Val_Enum.Is<EPropertyBagPropertyType>());
	TestTrue("Blood is I32", Val_I32.Is<int32>());
	TestTrue("Blood is I32 (Wrapper)", Val_I32.Is<FBloodI32>());
	TestTrue("Blood is I64", Val_I64.Is<int64>());

	TestTrue("Blood is Name", Val_Name.Is<FName>());
	TestTrue("Blood is String", Val_String.Is<FString>());
	TestTrue("Blood is Text", Val_Text.Is<FText>());

	TestTrue("Blood is Boolean Array", Val_BoolArray.Is<TArray<bool>>());
	TestTrue("Blood is Vector Array", Val_VectorArray.Is<TArray<FVector>>());
	TestTrue("Blood is Vector3f Array", Val_Vector3fArray.Is<TArray<FVector3f>>());
	TestTrue("Blood is Class Set", Val_ClassSet.Is<TSet<TSubclassOf<UObject>>>());

	TestTrue("Blood is I32/String Map", Val_I32StringMap.Is<TMap<int32, FString>>());
	TestTrue("Blood is Enum/Struct Map", Val_EnumStructMap.Is<TMap<EPropertyBagPropertyType, FVector3f>>());

	TestTrue("Blood as True", Val_BoolTrue.GetValue<bool>());
	TestFalse("Blood as False", Val_BoolFalse.GetValue<bool>());
	TestEqual("Blood as Byte", Val_Byte.GetValue<uint8>(), 137);
	TestEqual("Blood as Enum", Val_Enum.GetValue<EPropertyBagPropertyType>(), EPropertyBagPropertyType::Enum);
	TestEqual("Blood as I32", Val_I32.GetValue<int32>(), 10259);
	TestEqual("Blood as I32 (Wrapper)", Val_I32.GetValue<FBloodI32>(), 10259);
	TestEqual("Blood as I64", Val_I64.GetValue<int64>(), 8689101304);

	TestEqual("Blood as Name", Val_Name.GetValue<FName>(), FName("Hello, World!"));
	TestEqual("Blood as String", Val_String.GetValue<FString>(), "Hello, World!");
	TestEqual("Blood as Text", Val_Text.GetValue<FText>().ToString(), FText::FromString("Hello, World!").ToString());

	TestEqual("Blood as Boolean Array", Val_BoolArray.GetValue<TArray<bool>>(), TestBoolArray);
	TestEqual("Blood as Vector Array", Val_VectorArray.GetValue<TArray<FVector>>(), TestVectorArray);
	TestEqual("Blood as Vector3f Array", Val_Vector3fArray.GetValue<TArray<FVector3f>>(), TestVector3fArray);
	TestEqual("Blood as Class Set", Val_ClassSet.GetValue<TSet<TSubclassOf<UObject>>>().Array(), TestClassSet.Array());

	TestTrue("Blood as I32/String Map", TestMap1.OrderIndependentCompareEqual(Val_I32StringMap.GetValue<TMap<int32, FString>>()));
	TestTrue("Blood as Enum/Struct Map", TestMap2.OrderIndependentCompareEqual(Val_EnumStructMap.GetValue<TMap<EPropertyBagPropertyType, FVector3f>>()));

	// Un-set values should not not be value
	FBloodValue Val_ResetValid;
	TestFalse("Blood is invalid", Val_ResetValid.IsValid());

	// After being set, rest validity
	Val_ResetValid = Blood::ToBloodValue<int32>(10259);
	TestTrue("Blood is valid", Val_ResetValid.IsValid());

	// Reset should kill validity
	Val_ResetValid.Reset();
	TestFalse("Blood is invalid after reset", Val_ResetValid.IsValid());

	return true;
}