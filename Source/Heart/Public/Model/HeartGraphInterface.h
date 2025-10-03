// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartGraphInterface.generated.h"

struct FHeartNodeGuid;
class UHeartGraph;

UINTERFACE(NotBlueprintable)
class UHeartGraphInterface : public UInterface
{
	GENERATED_BODY()
};

class HEART_API IHeartGraphInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	virtual UHeartGraph* GetHeartGraph() const PURE_VIRTUAL(IHeartGraphInterface::GetHeartGraph, return nullptr; )

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location (Guid)"))
	virtual FVector2D GetNodeLocation(const FHeartNodeGuid& Node) const;

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location (Guid)"))
	virtual void SetNodeLocation(const FHeartNodeGuid& Node, const FVector2D& Location, bool InProgressMove);
};

UINTERFACE(NotBlueprintable)
class UHeartGraphInterface3D : public UHeartGraphInterface
{
	GENERATED_BODY()
};

class HEART_API IHeartGraphInterface3D : public IHeartGraphInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location 3D (Guid)"))
	virtual FVector GetNodeLocation3D(const FHeartNodeGuid& Node) const;

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location 3D (Guid)"))
	virtual void SetNodeLocation3D(const FHeartNodeGuid& Node, const FVector& Location, bool InProgressMove);
};


UINTERFACE()
class UHeartGraphInterfaceBP : public UHeartGraphInterface
{
	GENERATED_BODY()
};

class IHeartGraphInterfaceBP : public IHeartGraphInterface
{
	GENERATED_BODY()

protected:
	// Defer to Blueprint implementation
	virtual UHeartGraph* GetHeartGraph() const override final
	{
		return Execute_GetHeartGraph_BP(Cast<UObject>(this));
	}

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|Graph", meta = (DisplayName = "Get Heart Graph"))
	UHeartGraph* GetHeartGraph_BP() const;
};


class UHeartGraphNode;
class UHeartGraphNode3D;

// @todo probably remove this? we don't want to encourage using UHeartGraphNode instead of FHeartNodeGuid
UCLASS()
class HEART_API UHeartNodeLocationAccessorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location"))
	static FVector2D GetNodeLocation_Pointer(const TScriptInterface<IHeartGraphInterface>& Accessor, UHeartGraphNode* Node);

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location"))
	static void SetNodeLocation_Pointer(const TScriptInterface<IHeartGraphInterface>& Accessor,
		UHeartGraphNode* Node, const FVector2D& Location, bool InProgressMove);

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location 3D"))
	static FVector GetNodeLocation3D_Pointer(const TScriptInterface<IHeartGraphInterface3D>& Accessor, UHeartGraphNode3D* Node);

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location 3D"))
	static void SetNodeLocation3D_Pointer(const TScriptInterface<IHeartGraphInterface3D>& Accessor,
		UHeartGraphNode3D* Node, const FVector& Location, bool InProgressMove);
};