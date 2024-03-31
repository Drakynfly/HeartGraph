// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "HeartInputHandlerAssetFactory.generated.h"

UCLASS(hidecategories = Object, MinimalAPI)
class UHeartInputHandlerAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UHeartInputHandlerAssetFactory(const FObjectInitializer& ObjectInitializer);

	// UFactory
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// --

	// The class of the created asset to create
	UPROPERTY(EditAnywhere, Category = "HeartInputHandlerAssetFactory")
	TSubclassOf<class UHeartInputHandlerAssetBase> AssetClass;
};