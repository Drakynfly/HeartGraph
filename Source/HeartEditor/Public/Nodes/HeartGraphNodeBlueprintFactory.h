// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "HeartGraphNodeBlueprintFactory.generated.h"

UCLASS(hidecategories = Object, MinimalAPI)
class UHeartGraphNodeBlueprintFactory : public UFactory
{
	GENERATED_BODY()

public:
	UHeartGraphNodeBlueprintFactory(const FObjectInitializer& ObjectInitializer);

	// UFactory
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// --

	// The parent class of the created blueprint
	UPROPERTY(EditAnywhere, Category = "HeartGraphNodeBlueprintFactory")
	TSubclassOf<class UHeartGraphNode> ParentClass;
};
