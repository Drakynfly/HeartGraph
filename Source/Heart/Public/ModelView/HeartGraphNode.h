// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Model/HeartGuids.h"
#include "HeartGraphNode.generated.h"

class UHeartGraph;
class UHeartGraphPin;

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class HEART_API UHeartGraphNode : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

	void NewGuid();

	template <typename TUHeartGraphClass>
	TUHeartGraphClass* GetOwningGraph() const
	{
		return Cast<TUHeartGraphClass>(GetOuter());
	}

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	UHeartGraph* GetGraph() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|Node", meta = (DeterminesOutputType = "Class"))
	UHeartGraph* GetGraphTyped(TSubclassOf<UHeartGraph> Class) const { return GetGraph(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	FHeartNodeGuid GetGuid() const { return Guid; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|Node")
	UClass* GetSupportedClass() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	FVector2D GetLocation() const { return FVector2D(Location); }

	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	void SetLocation(const FVector2D& NewLocation);

	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	FVector GetLocation3D() const { return Location; }

	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	void SetLocation3D(const FVector& NewLocation);

	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	UHeartGraphPin* GetPin(const FHeartPinGuid& PinGuid);

	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	void AddPin(UHeartGraphPin* Pin);

	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	bool RemovePin(UHeartGraphPin* Pin);

protected:
	UPROPERTY(SaveGame)
	FHeartNodeGuid Guid;

	// Location is stored as a FVector for cases where a third dimensions is wanted.
	UPROPERTY(SaveGame)
	FVector Location;

	UPROPERTY(SaveGame, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<FHeartPinGuid, TObjectPtr<UHeartGraphPin>> Pins;
};
