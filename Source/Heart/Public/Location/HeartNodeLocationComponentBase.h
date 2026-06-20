// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartNodeLocationInterface.h"
#include "Model/HeartGraphComponentBase.h"
#include "HeartNodeLocationComponentBase.generated.h"

struct FInstancedStruct;

USTRUCT()
struct FHeartNodeLocationMigrationData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FVector> Locations;
};

/**
 * 
 */
UCLASS(Abstract)
class HEART_API UHeartNodeLocationComponentBase : public UHeartGraphComponentBase, public IHeartNodeLocationInterface
{
	GENERATED_BODY()

public:
	virtual void ExportMigrationData(FInstancedStruct& OutData) {}
	virtual void ImportMigrationData(const FInstancedStruct& InData) {}
};
