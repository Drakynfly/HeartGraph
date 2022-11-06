// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UI/HeartWidgetInputBindingContainer.h"
#include "HeartPinActionBinding.generated.h"

class UHeartGraphActionBase;

UCLASS(Abstract)
class UHeartGraphActionBinding : public UHeartWidgetInputEvent
{
	GENERATED_BODY()

public:
	virtual FHeartWidgetLinkedInput CreateDelegate() const override;

	virtual UObject* GetRelevantObject(UWidget* Widget) const;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartGraphActionBase> ActionClass;
};

UCLASS()
class HEART_API UAddGraphActionBinding : public UHeartGraphActionBinding
{
	GENERATED_BODY()

public:
	virtual UObject* GetRelevantObject(UWidget* Widget) const override;
};

UCLASS()
class HEART_API UAddNodeActionBinding : public UHeartGraphActionBinding
{
	GENERATED_BODY()

public:
	virtual UObject* GetRelevantObject(UWidget* Widget) const override;
};

UCLASS()
class HEART_API UAddPinActionBinding : public UHeartGraphActionBinding
{
	GENERATED_BODY()

public:
	virtual UObject* GetRelevantObject(UWidget* Widget) const override;
};

UCLASS()
class HEART_API UAddCanvasGraphActionBinding : public UHeartGraphActionBinding
{
	GENERATED_BODY()

public:
	virtual UObject* GetRelevantObject(UWidget* Widget) const override;
};

UCLASS()
class HEART_API UAddCanvasNodeActionBinding : public UHeartGraphActionBinding
{
	GENERATED_BODY()

public:
	virtual UObject* GetRelevantObject(UWidget* Widget) const override;
};

UCLASS()
class HEART_API UAddCanvasPinActionBinding : public UHeartGraphActionBinding
{
	GENERATED_BODY()

public:
	virtual UObject* GetRelevantObject(UWidget* Widget) const override;
};