// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "HeartGraphFactory.generated.h"

UCLASS(HideCategories = Object)
class HEARTEDITOR_API UHeartGraphFactory : public UFactory
{
	GENERATED_BODY()

public:
	UHeartGraphFactory(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = Asset)
	TSubclassOf<class UHeartGraph> AssetClass;

	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
