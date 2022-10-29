// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Templates/SubclassOf.h"

#include "HeartWidgetFactory.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class HEARTCORE_API UHeartWidgetFactory : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "HeartWidgetFactory")
	TSubclassOf<UUserWidget> FindWidgetClassForData(const UObject* Data) const;
};

USTRUCT(BlueprintType)
struct FHeartWidgetFactoryRules
{
	GENERATED_BODY()

	TSubclassOf<UUserWidget> GetWidgetClass(const UObject* Data) const;

protected:
	UPROPERTY(EditAnywhere, Instanced, Category = "HeartWidgetFactoryRules")
	TArray<TObjectPtr<UHeartWidgetFactory>> FactoryRules;
};

UCLASS()
class UHeartWidgetFactoryLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HeartWidgetFactoryLibrary")
	static TSubclassOf<UUserWidget> GetWidgetClass(const FHeartWidgetFactoryRules& Rules, const UObject* Data);
};
