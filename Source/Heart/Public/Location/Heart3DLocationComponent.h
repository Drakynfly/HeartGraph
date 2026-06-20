// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartNodeLocationComponentBase.h"
#include "HeartNodeLocationInterface.h"
#include "Heart3DLocationComponent.generated.h"

/**
 *
 */
UCLASS()
class HEART_API UHeart3DLocationComponent : public UHeartNodeLocationComponentBase, public IHeartGraphInterface3D
{
	GENERATED_BODY()

public:
	virtual void ExportMigrationData(FInstancedStruct& OutData) override;
	virtual void ImportMigrationData(const FInstancedStruct& InData) override;

	virtual FVector2D GetNodeLocation(const FHeartNodeGuid& Node) const override;
	virtual void SetNodeLocation(const FHeartNodeGuid& Node, const FVector2D& Location, bool InProgressMove) override;

	virtual FVector GetNodeLocation3D(const FHeartNodeGuid& Node) const override;
	virtual void SetNodeLocation3D(const FHeartNodeGuid& Node, const FVector& Location, bool InProgressMove) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "2DLocationComponent")
	TArray<FVector> Locations;
};
