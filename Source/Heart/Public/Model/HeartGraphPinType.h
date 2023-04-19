// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"

#include "HeartGraphPinType.generated.h"

USTRUCT(BlueprintType)
struct FHeartGraphPinData
{
	GENERATED_BODY()

	virtual ~FHeartGraphPinData() {}

	virtual bool IsValid() const
	{
		return true;
	}
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinValue : public FHeartGraphPinData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/Blood.BloodDataBase", ExcludeBaseStruct))
	FInstancedStruct ValueType;

	virtual bool IsValid() const override
	{
		return ValueType.IsValid();
	}
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinArray : public FHeartGraphPinData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/Blood.BloodDataBase", ExcludeBaseStruct))
	FInstancedStruct ArrayType;

	virtual bool IsValid() const override
	{
		return ArrayType.IsValid();
	}
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinSet : public FHeartGraphPinData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/Blood.BloodDataBase", ExcludeBaseStruct))
	FInstancedStruct SetType;

	virtual bool IsValid() const override
	{
		return SetType.IsValid();
	}
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinMap : public FHeartGraphPinData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/Blood.BloodDataBase", ExcludeBaseStruct))
	FInstancedStruct KeyType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/Blood.BloodDataBase", ExcludeBaseStruct))
	FInstancedStruct ValueType;

	virtual bool IsValid() const override
	{
		return KeyType.IsValid() && ValueType.IsValid();
	}
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinType // Based on FEdGraphPinType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/Heart.HeartGraphPinData"))
	FInstancedStruct Container;

	bool IsValidType() const
	{
		if (Container.IsValid())
		{
			if (!Container.Get<FHeartGraphPinData>().IsValid())
			{
				return false;
			}
		}
		return true;
	}
};