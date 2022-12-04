// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once
#include "InstancedStruct.h"
#include "InstancedStructArray.h"

#include "HeartGraphPinType.generated.h"

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
USTRUCT()
struct HEART_API FHeartGraphPinValue_POD : public FHeartGraphPinValueBase
{
	GENERATED_BODY()
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinBool : public FHeartGraphPinValue_POD
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Value = false;
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinDouble : public FHeartGraphPinValue_POD
{
	GENERATED_BODY()

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
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEART_API FHeartGraphPinSet : public FHeartGraphPinContainerBase
{
	GENERATED_BODY()
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


