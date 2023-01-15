// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Templates/SubclassOf.h"

#include "HeartWidgetFactory.generated.h"

// Shamelessly adapted from Lyra. Its just so useful :)


UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, CollapseCategories)
class HEARTCORE_API UHeartWidgetFactory : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|WidgetFactory")
	TSubclassOf<UUserWidget> FindWidgetClassForData(const UObject* Data) const;
};

USTRUCT(BlueprintType)
struct HEARTCORE_API FHeartWidgetFactoryRules
{
	GENERATED_BODY()

	TSubclassOf<UUserWidget> GetWidgetClass(const UObject* Data) const;

protected:
	UPROPERTY(EditAnywhere, Instanced, Category = "WidgetFactoryRules")
	TArray<TObjectPtr<UHeartWidgetFactory>> FactoryRules;
};

UCLASS()
class UHeartWidgetFactoryLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetFactoryLibrary")
	static TSubclassOf<UUserWidget> GetWidgetClass(const FHeartWidgetFactoryRules& Rules, const UObject* Data);
};
