// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartFlakeSaveFile.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartFlakeSaveFile)

void UHeartFlakeSaveFile::SetObjectToSave(UObject* Obj)
{
	Flake = Heart::Flakes::CreateFlake(Obj);
}

void UHeartFlakeSaveFile::SetStructToSave(const FInstancedStruct& Data)
{
	Flake = Heart::Flakes::CreateFlake(Data);
}

UObject* UHeartFlakeSaveFile::LoadObjectFromData(UObject* Outer) const
{
	return Heart::Flakes::CreateObject(Flake, Outer, UObject::StaticClass());
}

UObject* UHeartFlakeSaveFile::LoadObjectFromDataClassChecked(UObject* Outer, const UClass* ExpectedClass) const
{
	if (auto&& Obj = LoadObjectFromData(Outer))
	{
		check(Obj->GetClass()->IsChildOf(ExpectedClass));
		return Obj;
	}

	return nullptr;
}

void UHeartFlakeSaveFile::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	UE_LOG(LogTemp, Log, TEXT("Serialize for RoseSaveFile called with Archive in state: %s"),
		Ar.IsLoading() ? TEXT("Loading") : Ar.IsSaving() ? TEXT("Saving") : TEXT("Unknown"));

	Ar << Flake;

	UE_LOG(LogTemp, Log, TEXT("Serialize for RoseSaveFile class: %s"),
		*Flake.Struct.ToString());
}