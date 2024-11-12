// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartJsonSerializer.h"
#include "JsonObjectConverter.h"
#include "Model/HeartGraphNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartJsonSerializer)

namespace CopiedFromJsonObjectConverter
{
	const FString ObjectClassNameKey = "_ClassName";
}

//@ todo UE has no equivalent Import Callback, so this is severely limited currently
// we either need to wait for unreal to merge this PR: https://github.com/EpicGames/UnrealEngine/pull/11170,
// or implement a solution ourselves, if we want to use this callback for anything beyond minor tweaks that can be auto-imported.
static const FJsonObjectConverter::CustomExportCallback HeartJsonCustomExporter =
	FJsonObjectConverter::CustomExportCallback::CreateLambda(
		[](FProperty* Property, const void* Value) -> TSharedPtr<FJsonValue>
		{
			// @todo Uncomment this once the above has been resolved one way or the other
			/*
			if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
			{
				static const TArray<FName> GuidStructs {
					FHeartGuid::StaticStruct()->GetFName(),
					FHeartGraphGuid::StaticStruct()->GetFName(),
					FHeartExtensionGuid::StaticStruct()->GetFName(),
					FHeartNodeGuid::StaticStruct()->GetFName(),
					FHeartPinGuid::StaticStruct()->GetFName(), };

				// Export our guid types a little more neatly then unreal does by default.
				if (StructProperty->Struct && GuidStructs.Contains(StructProperty->Struct->GetFName()))
				{
					FGuid* GuidValue;
					StructProperty->GetValue_InContainer(Value, &GuidValue);
					if (GuidValue)
					{
						return MakeShared<FJsonValueString>(GuidValue->ToString(EGuidFormats::Short));
					}
				}
			}
			else */if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
			{
				// @todo make this generic for any Object, the move this module to Flakes plugin wholesale!

				// Custom handler for UHeartGraphNode::NodeObject, because we want to switch between treating it as a reference
				// and a subobject to recurse depending on ownership.
				if (ObjectProperty->GetName() == TEXT("NodeObject"))
				{
					UObject* NodeObject = ObjectProperty->GetObjectPropertyValue(Value);

					// If we own the NodeObject, export by recursing over its data, e.g., export the full object data.
					if (NodeObject && NodeObject->GetOuter()->IsA<UHeartGraphNode>())
					{
						TSharedRef<FJsonObject> Out = MakeShared<FJsonObject>();

						Out->SetStringField(CopiedFromJsonObjectConverter::ObjectClassNameKey, NodeObject->GetClass()->GetPathName());
						if (FJsonObjectConverter::UStructToJsonObject(ObjectProperty->GetObjectPropertyValue(Value)->GetClass(), NodeObject, Out, 0, 0, &HeartJsonCustomExporter))
						{
							TSharedRef<FJsonValueObject> JsonObject = MakeShared<FJsonValueObject>(Out);
							JsonObject->Type = EJson::Object;
							return JsonObject;
						}
					}
					// If it's a reference, export the object path.
					else
					{
						FString StringValue;
						Property->ExportTextItem_Direct(StringValue, Value, nullptr, nullptr, PPF_None);
						return MakeShared<FJsonValueString>(StringValue);
					}
				}
			}

			return {};
		});

namespace Flakes
{
	void FSerializationProvider_Json::Static_ReadData(const FConstStructView& Struct, TArray<uint8>& OutData)
	{
		FString StringData;
		FJsonObjectConverter::UStructToJsonObjectString(Struct.GetScriptStruct(), Struct.GetMemory(),
			StringData,
			0,
			0,
			0,
			&HeartJsonCustomExporter,
			false);

		OutData.AddUninitialized(StringData.Len());
		StringToBytes(StringData, OutData.GetData(), StringData.Len());
	}

	void FSerializationProvider_Json::Static_ReadData(const UObject* Object, TArray<uint8>& OutData)
	{
		FString StringData;
		FJsonObjectConverter::UStructToJsonObjectString(Object->GetClass(), Object,
			StringData,
			0,
			0,
			0,
			&HeartJsonCustomExporter,
			false);

		OutData.AddUninitialized(StringData.Len());
		StringToBytes(StringData, OutData.GetData(), StringData.Len());
	}

	void FSerializationProvider_Json::Static_WriteData(const FStructView& Struct, const TArray<uint8>& Data)
	{
		if (!ensure(Struct.IsValid()))
		{
			return;
		}

		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(BytesToString(Data.GetData(), Data.Num()));

		TSharedPtr<FJsonObject> JsonObject;
		if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
		{
			return;
		}

		constexpr int64 CheckFlags = 0;
		constexpr int64 SkipFlags = 0;
		constexpr bool bStrictMode = false;

		FJsonObjectConverter::JsonObjectToUStruct(
			JsonObject.ToSharedRef(),
			Struct.GetScriptStruct(),
			Struct.GetMemory(),
			CheckFlags,
			SkipFlags,
			bStrictMode);
	}

	void FSerializationProvider_Json::Static_WriteData(UObject* Object, const TArray<uint8>& Data)
	{
		if (!ensure(IsValid(Object)))
		{
			return;
		}

		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(BytesToString(Data.GetData(), Data.Num()));

		TSharedPtr<FJsonObject> JsonObject;
		if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
		{
			return;
		}

		constexpr int64 CheckFlags = 0;
		constexpr int64 SkipFlags = 0;
		constexpr bool bStrictMode = false;

		FJsonObjectConverter::JsonObjectToUStruct(
			JsonObject.ToSharedRef(),
			Object->GetClass(),
			Object,
			CheckFlags,
			SkipFlags,
			bStrictMode);
	}

	FName FSerializationProvider_Json::GetProviderName()
	{
		static const FLazyName JsonSerializationProvider("JSON");
		return JsonSerializationProvider;
	}
}

FJsonObjectWrapper UHeartJsonLibrary::CreateFlake_Struct_Json(const FInstancedStruct& Struct)
{
	FJsonObjectWrapper Wrapper;
	Wrapper.JsonObject = MakeShared<FJsonObject>();
	FJsonObjectConverter::UStructToJsonObject(Struct.GetScriptStruct(), Struct.GetMemory(),
		Wrapper.JsonObject.ToSharedRef(),
		0,
		0,
		&HeartJsonCustomExporter);
	return Wrapper;
}

FJsonObjectWrapper UHeartJsonLibrary::CreateFlake_Json(const UObject* Object)
{
	FJsonObjectWrapper Wrapper;
	Wrapper.JsonObject = MakeShared<FJsonObject>();
	FJsonObjectConverter::UStructToJsonObject(Object->GetClass(), Object,
		Wrapper.JsonObject.ToSharedRef(),
		0,
		0,
		&HeartJsonCustomExporter);
	return Wrapper;
}

FJsonObjectWrapper UHeartJsonLibrary::CreateFlake_Actor_Json(const AActor* Actor)
{
	FJsonObjectWrapper Wrapper;
	Wrapper.JsonObject = MakeShared<FJsonObject>();
	FJsonObjectConverter::UStructToJsonObject(Actor->GetClass(), Actor,
		Wrapper.JsonObject.ToSharedRef(),
		0,
		0,
		&HeartJsonCustomExporter);
	return Wrapper;
}

UObject* UHeartJsonLibrary::ConstructObjectFromFlake_Json(const FJsonObjectWrapper& Flake, UObject* Outer,
	const UClass* ExpectedClass)
{
	UObject* Obj = NewObject<UObject>(Outer, ExpectedClass);
	FJsonObjectConverter::JsonObjectToUStruct(Flake.JsonObject.ToSharedRef(), ExpectedClass, Obj);
	return Obj;
}

AActor* UHeartJsonLibrary::ConstructActorFromFlake_Json(const FJsonObjectWrapper& Flake, UObject* WorldContextObj,
	const TSubclassOf<AActor> ExpectedClass)
{
	if (auto&& LoadedActor = WorldContextObj->GetWorld()->SpawnActorDeferred<AActor>(ExpectedClass, FTransform::Identity))
	{
		FJsonObjectConverter::JsonObjectToUStruct(Flake.JsonObject.ToSharedRef(), ExpectedClass, LoadedActor);

		LoadedActor->FinishSpawning(LoadedActor->GetTransform());

		return LoadedActor;
	}

	return nullptr;
}

FString UHeartJsonLibrary::ToString(const FJsonObjectWrapper& Json, const bool bPretty)
{
	FString JsonString;
	if (bPretty)
	{
		TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&JsonString);
		FJsonSerializer::Serialize(Json.JsonObject.ToSharedRef(), JsonWriter);
		JsonWriter->Close();
	}
	else
	{
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonString);
		FJsonSerializer::Serialize(Json.JsonObject.ToSharedRef(), JsonWriter);
		JsonWriter->Close();
	}
	return JsonString;
}