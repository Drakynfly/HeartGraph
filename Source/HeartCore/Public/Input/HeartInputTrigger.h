// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InputCoreTypes.h"
#include "Input/HeartInputTrip.h"
#include "HeartInputTrigger.generated.h"

USTRUCT(BlueprintType)
struct HEARTCORE_API FHeartInputTrigger
{
	GENERATED_BODY()

	virtual ~FHeartInputTrigger() {}

	virtual TArray<Heart::Input::FInputTrip> CreateTrips() const { return TArray<Heart::Input::FInputTrip>(); }
};

USTRUCT(BlueprintType, meta = (DisplayName = "Key Down"))
struct FHeartInputTrigger_KeyDown : public FHeartInputTrigger
{
	GENERATED_BODY()

	virtual TArray<Heart::Input::FInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	TSet<FKey> Keys;
};

USTRUCT(BlueprintType)
struct FHeartKeyAndModifiers
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	FKey Key;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	uint8 WithShift : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	uint8 WithControl : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	uint8 WithAlt : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	uint8 WithCommand : 1;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Key Down (with modifiers)"))
struct FHeartInputTrigger_KeyDownMod : public FHeartInputTrigger
{
	GENERATED_BODY()

	virtual TArray<Heart::Input::FInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	TArray<FHeartKeyAndModifiers> Keys;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Key Up"))
struct FHeartInputTrigger_KeyUp : public FHeartInputTrigger
{
	GENERATED_BODY()

	virtual TArray<Heart::Input::FInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyUp")
	TSet<FKey> Keys;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Manual"))
struct FHeartInputTrigger_Manual : public FHeartInputTrigger
{
	GENERATED_BODY()

	virtual TArray<Heart::Input::FInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Manual")
	TSet<FName> Keys;
};