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
struct HEARTCORE_API FHeartInputTrigger_KeyDown : public FHeartInputTrigger
{
	GENERATED_BODY()

	virtual TArray<Heart::Input::FInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	TSet<FKey> Keys;
};

USTRUCT(BlueprintType)
struct HEARTCORE_API FHeartKeyAndModifiers
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	FKey Key;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	bool WithShift = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	bool WithControl = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	bool WithAlt = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	bool WithCommand = false;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Key Down (with modifiers)"))
struct HEARTCORE_API FHeartInputTrigger_KeyDownMod : public FHeartInputTrigger
{
	GENERATED_BODY()

	virtual TArray<Heart::Input::FInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyDown")
	TArray<FHeartKeyAndModifiers> Keys;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Key Up"))
struct HEARTCORE_API FHeartInputTrigger_KeyUp : public FHeartInputTrigger
{
	GENERATED_BODY()

	virtual TArray<Heart::Input::FInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyUp")
	TSet<FKey> Keys;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Key Up (with modifiers)"))
struct HEARTCORE_API FHeartInputTrigger_KeyUpMod : public FHeartInputTrigger
{
	GENERATED_BODY()

	virtual TArray<Heart::Input::FInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyUp")
	TSet<FHeartKeyAndModifiers> Keys;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Manual"))
struct HEARTCORE_API FHeartInputTrigger_Manual : public FHeartInputTrigger
{
	GENERATED_BODY()

	virtual TArray<Heart::Input::FInputTrip> CreateTrips() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Manual")
	TSet<FName> Keys;
};