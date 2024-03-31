// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "GameFramework/SaveGame.h"
#include "HeartFlakes.h"

#include "HeartFlakeSaveFile.generated.h"

/**
 *
 */
UCLASS()
class HEARTCORE_API UHeartFlakeSaveFile : public USaveGame
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|FlakeSave")
	void SetObjectToSave(UObject* Obj);

	UFUNCTION(BlueprintCallable, Category = "Heart|FlakeSave")
	void SetStructToSave(const FInstancedStruct& Data);

	UFUNCTION(BlueprintCallable, Category = "Heart|FlakeSave")
	UObject* LoadObjectFromData(UObject* Outer) const;

	template <typename TClass>
	TClass* LoadObjectFromData(UObject* Outer = GetTransientPackage())
	{
		if (auto Obj = LoadObjectFromDataClassChecked(Outer, TClass::StaticClass()))
		{
			return Cast<TClass>(Obj);
		}

		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|FlakeSave")
	UObject* LoadObjectFromDataClassChecked(UObject* Outer, const UClass* ExpectedClass) const;

private:
	UPROPERTY()
	FHeartFlake Flake;

	virtual void Serialize(FArchive& Ar) override;
};