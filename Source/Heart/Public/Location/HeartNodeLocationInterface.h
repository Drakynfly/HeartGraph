// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartNodeLocationInterface.generated.h"

struct FHeartNodeGuid;

UINTERFACE(NotBlueprintable)
class UHeartNodeLocationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class HEART_API IHeartNodeLocationInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location (Guid)"))
	virtual FVector2D GetNodeLocation(const FHeartNodeGuid& Node) const
		PURE_VIRTUAL(IHeartNodeLocationInterface::GetNodeLocation, return FVector2D(); )

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location (Guid)"))
	virtual void SetNodeLocation(const FHeartNodeGuid& Node, const FVector2D& Location, bool InProgressMove)
		PURE_VIRTUAL(IHeartNodeLocationInterface::SetNodeLocation, )

	// @todo temp until event handler system is added to HeartGraph for extensions/features to use.
	virtual void NotifyNodeLocationsChanged(const TSet<FHeartNodeGuid>& AffectedNodes, bool InProgress) {}
};

UINTERFACE(NotBlueprintable)
class UHeartGraphInterface3D : public UHeartNodeLocationInterface
{
	GENERATED_BODY()
};

class HEART_API IHeartGraphInterface3D : public IHeartNodeLocationInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location 3D (Guid)"))
	virtual FVector GetNodeLocation3D(const FHeartNodeGuid& Node) const
		PURE_VIRTUAL(IHeartGraphInterface3D::GetNodeLocation3D, return FVector(); )

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location 3D (Guid)"))
	virtual void SetNodeLocation3D(const FHeartNodeGuid& Node, const FVector& Location, bool InProgressMove)
		PURE_VIRTUAL(IHeartGraphInterface3D::SetNodeLocation3D, )
};

class UHeartGraphNode;
class UHeartGraphNode3D;

// @Deprecated we don't want to use UHeartGraphNode instead of FHeartNodeGuid
UCLASS()
class UHeartNodeLocationAccessorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UE_DEPRECATED(5.7, "Use the guid call directly on the interface")
	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location"))
	static FVector2D GetNodeLocation_Pointer(const TScriptInterface<IHeartNodeLocationInterface>& Accessor, UHeartGraphNode* Node);

	UE_DEPRECATED(5.7, "Use the guid call directly on the interface")
	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location"))
	static void SetNodeLocation_Pointer(const TScriptInterface<IHeartNodeLocationInterface>& Accessor,
		UHeartGraphNode* Node, const FVector2D& Location, bool InProgressMove);

	UE_DEPRECATED(5.7, "Use the guid call directly on the interface")
	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location 3D"))
	static FVector GetNodeLocation3D_Pointer(const TScriptInterface<IHeartGraphInterface3D>& Accessor, UHeartGraphNode3D* Node);

	UE_DEPRECATED(5.7, "Use the guid call directly on the interface")
	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location 3D"))
	static void SetNodeLocation3D_Pointer(const TScriptInterface<IHeartGraphInterface3D>& Accessor,
		UHeartGraphNode3D* Node, const FVector& Location, bool InProgressMove);
};

class IHeartGraphNodeInterface;

namespace Heart::Features::Location
{
	HEART_API FVector2D GetNodeLocation(const IHeartGraphNodeInterface& Node);
}
