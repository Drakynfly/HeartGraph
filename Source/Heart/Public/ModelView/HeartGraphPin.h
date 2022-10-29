// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Model/HeartGuids.h"
#include "Model/HeartGraphPinReference.h"
#include "HeartGraphPin.generated.h"

class UHeartGraphNode;

/**
 *
 */
UCLASS()
class HEART_API UHeartGraphPin : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

	void NewGuid();

	template <typename THeartNodeClass>
	THeartNodeClass* GetOwningNode() const
	{
		return Cast<THeartNodeClass>(GetOuter());
	}

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	UHeartGraphNode* GetNode() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|Node", meta = (DeterminesOutputType = "Class"))
	UHeartGraphNode* GetNodeTyped(TSubclassOf<UHeartGraphNode> Class) const { return GetNode(); }

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|Pin")
	FHeartPinGuid GetGuid() const { return Guid; }

	UFUNCTION(BlueprintCallable, Category = "Heart|Pin")
	FHeartGraphPinReference GetReference() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|Pin")
	const TArray<FHeartGraphPinReference>& GetLinks() const { return Links; }

	UFUNCTION(BlueprintCallable, Category = "Heart|Pin")
	bool IsConnected() const { return !Links.IsEmpty(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|Pin")
	int32 GetNumLinks() const { return Links.Num(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|Pin")
	TArray<UHeartGraphPin*> GetAllConnections();

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|Pin")
	UHeartGraphPin* ResolveConnection(const int32 Index) const;

private:
	UPROPERTY(SaveGame)
	FHeartPinGuid Guid;

	UPROPERTY(SaveGame, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FHeartGraphPinReference> Links;
};
