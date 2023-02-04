// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetInputTrip.h"
#include "HeartWidgetInputTrigger.generated.h"

USTRUCT(BlueprintType)
struct HEARTCORE_API FHeartWidgetInputTrigger
{
	GENERATED_BODY()

	virtual ~FHeartWidgetInputTrigger() {}

	virtual TArray<FHeartWidgetInputTrip> CreateTrips() const { return TArray<FHeartWidgetInputTrip>(); }
};

USTRUCT(BlueprintType, meta = (DisplayName = "Key Down"))
struct FHeartWidgetInputTrigger_KeyDown : public FHeartWidgetInputTrigger
{
	GENERATED_BODY()

	virtual TArray<FHeartWidgetInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FKey Key;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Key Up"))
struct FHeartWidgetInputTrigger_KeyUp : public FHeartWidgetInputTrigger
{
	GENERATED_BODY()

	virtual TArray<FHeartWidgetInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FKey Key;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Manual"))
struct FHeartWidgetInputTrigger_Manual : public FHeartWidgetInputTrigger
{
	GENERATED_BODY()

	virtual TArray<FHeartWidgetInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSet<FName> Keys;
};