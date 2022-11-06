// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Model/HeartGuids.h"
#include "Model/HeartGraphPinReference.h"
#include "Model/HeartPinDirection.h"
#include "HeartGraphPin.generated.h"

class UHeartGraphNode;

class UHeartGraphPin;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeartPinConnectionsChanged, UHeartGraphPin*, Pin);

/**
 *
 */
UCLASS(BlueprintType)
class HEART_API UHeartGraphPin : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

	void NewGuid();
	void SetDirection(EHeartPinDirection Direction);

	template <typename THeartNodeClass>
	THeartNodeClass* GetOwningNode() const
	{
		return Cast<THeartNodeClass>(GetOuter());
	}

	void ConnectTo(UHeartGraphPin* Other);

	void DisconnectFrom(const FHeartGraphPinReference Other, bool NotifyNode);

	void DisconnectFromAll(bool NotifyNodes);

public:
	UFUNCTION(BlueprintCallable, Category = "HeartGraphPin")
	UHeartGraphNode* GetNode() const;

	UFUNCTION(BlueprintCallable, Category = "HeartGraphPin", meta = (DeterminesOutputType = "Class"))
	UHeartGraphNode* GetNodeTyped(TSubclassOf<UHeartGraphNode> Class) const { return GetNode(); }

public:
	UFUNCTION(BlueprintCallable, Category = "HeartGraphPin")
	FHeartPinGuid GetGuid() const { return Guid; }

	UFUNCTION(BlueprintCallable, Category = "HeartGraphPin")
	FHeartGraphPinReference GetReference() const;

	UFUNCTION(BlueprintCallable, Category = "HeartGraphPin")
	EHeartPinDirection GetDirection() const { return PinDirection; }

	UFUNCTION(BlueprintCallable, Category = "HeartGraphPin")
	const TArray<FHeartGraphPinReference>& GetLinks() const { return Links; }

	UFUNCTION(BlueprintCallable, Category = "HeartGraphPin")
	bool IsConnected() const { return !Links.IsEmpty(); }

	UFUNCTION(BlueprintCallable, Category = "HeartGraphPin")
	int32 GetNumLinks() const { return Links.Num(); }

	UFUNCTION(BlueprintCallable, Category = "HeartGraphPin")
	TArray<UHeartGraphPin*> GetAllConnections();

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "HeartGraphPin")
	UHeartGraphPin* ResolveConnectionByReference(const FHeartGraphPinReference Reference) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "HeartGraphPin")
	UHeartGraphPin* ResolveConnection(const int32 Index) const;

public:
	UPROPERTY(BlueprintAssignable, Category = "HeartGraphPin|Events")
	FHeartPinConnectionsChanged OnPinConnectionsChanged;

private:
	UPROPERTY()
	FHeartPinGuid Guid;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EHeartPinDirection PinDirection;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FHeartGraphPinReference> Links;
};
