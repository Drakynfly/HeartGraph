// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"

#include "HeartGraphPinType.generated.h"

// @todo this entire method of generating pin types is kinda wonky, and deserves a rethink at some point

/**
 *
 */
USTRUCT()
struct HEART_API FHeartGraphPinValueBase
{
	GENERATED_BODY()

	virtual ~FHeartGraphPinValueBase() {}
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinBool : public FHeartGraphPinValueBase
{
	GENERATED_BODY()

	// @todo this isn't actually used anywhere . . .
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Value = false;
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinDouble : public FHeartGraphPinValueBase
{
	GENERATED_BODY()

	// @todo this isn't actually used anywhere . . .
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double Value = 0.0;
};


/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinContainerBase
{
	GENERATED_BODY()

	virtual ~FHeartGraphPinContainerBase() {}

	virtual bool IsValidContainer() const
	{
		return true;
	}
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinArray : public FHeartGraphPinContainerBase
{
	GENERATED_BODY()

	// This is just a flag type. Is marks the pin as using Array-style behavior.
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinSet : public FHeartGraphPinContainerBase
{
	GENERATED_BODY()

	// This is just a flag type. Is marks the pin as using Set-style behavior.
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinMap : public FHeartGraphPinContainerBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/Heart.HeartGraphPinValueBase", ExcludeBaseStruct))
	FInstancedStruct KeyValue;

	virtual bool IsValidContainer() const override
	{
		return KeyValue.IsValid();
	}
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinType // Based on FEdGraphPinType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/Heart.HeartGraphPinValueBase", ExcludeBaseStruct))
	FInstancedStruct Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/Heart.HeartGraphPinContainerBase", ExcludeBaseStruct))
	FInstancedStruct Container;

	bool IsValidType() const
	{
		if (Container.IsValid())
		{
			if (!Container.Get<FHeartGraphPinContainerBase>().IsValidContainer())
			{
				return false;
			}
		}

		return Value.IsValid();
	}
};