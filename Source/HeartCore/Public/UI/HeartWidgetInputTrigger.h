// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetInputTrip.h"
#include "HeartWidgetInputTrigger.generated.h"

USTRUCT(BlueprintType)
struct HEARTCORE_API FHeartWidgetInputTrigger
{
	GENERATED_BODY()

	virtual ~FHeartWidgetInputTrigger() {}

	virtual TArray<Heart::Input::FInputTrip> CreateTrips() const { return TArray<Heart::Input::FInputTrip>(); }
};

USTRUCT(BlueprintType, meta = (DisplayName = "Key Down"))
struct FHeartWidgetInputTrigger_KeyDown : public FHeartWidgetInputTrigger
{
	GENERATED_BODY()

	virtual TArray<Heart::Input::FInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	TSet<FKey> Keys;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Key Up"))
struct FHeartWidgetInputTrigger_KeyUp : public FHeartWidgetInputTrigger
{
	GENERATED_BODY()

	virtual TArray<Heart::Input::FInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyUp")
	TSet<FKey> Keys;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Manual"))
struct FHeartWidgetInputTrigger_Manual : public FHeartWidgetInputTrigger
{
	GENERATED_BODY()

	virtual TArray<Heart::Input::FInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Manual")
	TSet<FName> Keys;
};