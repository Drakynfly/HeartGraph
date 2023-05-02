// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetFactory.h"
#include "HeartWidgetFactory_Class.generated.h"

/**
 *
 */
UCLASS()
class HEARTCORE_API UHeartWidgetFactory_Class : public UHeartWidgetFactory
{
	GENERATED_BODY()

public:
	virtual TSubclassOf<UUserWidget> FindWidgetClassForData_Implementation(const UObject* Data) const override;

protected:
	UPROPERTY(EditAnywhere, Category = "WidgetFactoryClass", meta = (AllowAbstract))
	TMap<TSoftClassPtr<UObject>, TSubclassOf<UUserWidget>> EntryWidgetForClass;
};
