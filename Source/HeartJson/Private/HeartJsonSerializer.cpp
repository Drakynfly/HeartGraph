// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartJsonSerializer.h"
#include "JsonObjectConverter.h"
#include "Model/HeartGraphNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartJsonSerializer)

namespace CopiedFromJsonObjectConverter
{
	const FString ObjectClassNameKey = "_ClassName";
}

static const FJsonObjectConverter::CustomExportCallback HeartJsonCustomExporter =
	FJsonObjectConverter::CustomExportCallback::CreateLambda(
		[](FProperty* Property, const void* Value) -> TSharedPtr<FJsonValue>
		{
			if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
			{
				if (ObjectProperty->GetName() == TEXT("NodeObject"))
				{
					UObject* Object = ObjectProperty->GetObjectPropertyValue(Value);
					if (Object && Object->GetOuter()->IsA<UHeartGraphNode>())
					{
						TSharedRef<FJsonObject> Out = MakeShared<FJsonObject>();

						Out->SetStringField(CopiedFromJsonObjectConverter::ObjectClassNameKey, Object->GetClass()->GetPathName());
						if (FJsonObjectConverter::UStructToJsonObject(ObjectProperty->GetObjectPropertyValue(Value)->GetClass(), Object, Out, 0, 0, &HeartJsonCustomExporter))
						{
							TSharedRef<FJsonValueObject> JsonObject = MakeShared<FJsonValueObject>(Out);
							JsonObject->Type = EJson::Object;
							return JsonObject;
						}
					}
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