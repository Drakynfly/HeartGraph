// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "BloodValue.h"
#include "BloodContainer.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(BloodContainerTest,
								 "Blood.BloodContainerTest",
								 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool BloodContainerTest::RunTest(const FString& Parameters)
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

	FBloodContainer Container;

	TestTrue("Container is empty", Container.IsEmpty());

	//Container.AddBloodValue("Val_BoolTrue", Val_BoolTrue); // @todo this is borked somehow
	Container.Add("Val_BoolTrue", Val_BoolTrue.GetValue<bool>());

	Container.AddBloodValue("Val_BoolFalse", Val_BoolFalse);
	Container.AddBloodValue("Val_Byte", Val_Byte);
	Container.AddBloodValue("Val_Enum", Val_Enum);
	Container.AddBloodValue("Val_I32", Val_I32);
	Container.AddBloodValue("Val_I64", Val_I64);

	Container.AddBloodValue("Val_Name", Val_Name);
	Container.AddBloodValue("Val_String", Val_String);
	Container.AddBloodValue("Val_Text", Val_Text);

	Container.AddBloodValue("Val_BoolArray", Val_BoolArray);
	Container.AddBloodValue("Val_VectorArray", Val_VectorArray);
	Container.AddBloodValue("Val_Vector3fArray", Val_Vector3fArray);
	Container.AddBloodValue("Val_ClassSet", Val_ClassSet);

	Container.AddBloodValue("Val_I32StringMap", Val_I32StringMap);
	Container.AddBloodValue("Val_EnumStructMap", Val_EnumStructMap);

	TestTrue("Contains bool", Container.Contains("Val_BoolTrue"));
	TestTrue("Contains int32", Container.Contains("Val_I32"));
	TestFalse("Does not contain value", Container.Contains("Val_BoolTrue_No"));

	TestTrue("Get True", Container.Get<bool>("Val_BoolTrue"));
	TestFalse("Get False", Container.Get<bool>("Val_BoolFalse"));
	TestEqual("Get Byte", Container.Get<uint8>("Val_Byte"), 137);
	TestEqual("Get Enum", Container.Get<EPropertyBagPropertyType>("Val_Enum"), EPropertyBagPropertyType::Enum);
	TestEqual("Get I32", Container.Get<int32>("Val_I32"), 10259);
	TestEqual("Get I32 (Wrapper)", Container.Get<FBloodI32>("Val_I32"), 10259);
	TestEqual("Get I64", Container.Get<int64>("Val_I64"), int64(8689101304));

	TestEqual("Get Name", Container.Get<FName>("Val_Name"), FName("Hello, World!"));
	TestEqual("Get String", Container.Get<FString>("Val_String"), "Hello, World!");
	TestEqual("Get Text", Container.Get<FText>("Val_Text").ToString(), FText::FromString("Hello, World!").ToString());

	TestEqual("Get Boolean Array", Container.Get<TArray<bool>>("Val_BoolArray"), TestBoolArray);
	TestEqual("Get Vector Array", Container.Get<TArray<FVector>>("Val_VectorArray"), TestVectorArray);
	TestEqual("Get Vector3f Array", Container.Get<TArray<FVector3f>>("Val_Vector3fArray"), TestVector3fArray);
	TestEqual("Get Class Set", Container.Get<TSet<TSubclassOf<UObject>>>("Val_ClassSet").Array(), TestClassSet.Array());

	TestTrue("Get I32/String Map", TestMap1.OrderIndependentCompareEqual(Container.Get<TMap<int32, FString>>("Val_I32StringMap")));
	TestTrue("Get Enum/Struct Map", TestMap2.OrderIndependentCompareEqual(Container.Get<TMap<EPropertyBagPropertyType, FVector3f>>("Val_EnumStructMap")));

	TestFalse("Container is empty", Container.IsEmpty());
	Container.Clear();
	TestTrue("Container is empty", Container.IsEmpty());

	return true;
}

#endif