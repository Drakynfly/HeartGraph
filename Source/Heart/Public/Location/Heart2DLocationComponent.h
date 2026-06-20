// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartNodeLocationComponentBase.h"
#include "HeartNodeLocationInterface.h"
#include "Heart2DLocationComponent.generated.h"

/**
 * 
 */
UCLASS()
class HEART_API UHeart2DLocationComponent : public UHeartNodeLocationComponentBase
{
	GENERATED_BODY()

public:
	virtual void ExportMigrationData(FInstancedStruct& OutData) override;
	virtual void ImportMigrationData(const FInstancedStruct& InData) override;

	virtual FVector2D GetNodeLocation(const FHeartNodeGuid& Node) const override;
	virtual void SetNodeLocation(const FHeartNodeGuid& Node, const FVector2D& Location, bool InProgressMove) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "2DLocationComponent")
	TArray<FVector2D> Locations;
};
