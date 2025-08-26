// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Model/HeartGraphNodeComponent.h"
#include "Model/HeartGraphPinDesc.h"
#include "Model/HeartNodeComponentPinProvider.h"
#include "HeartInstancedPinsComponent.generated.h"

/**
 * 
 */
UCLASS()
class HEART_API UHeartInstancedPinsComponent : public UHeartGraphNodeComponent, public IHeartNodeComponentPinProvider
{
	GENERATED_BODY()

public:
	//~ IHeartNodeComponentPinProvider
	virtual void GatherPins(TArray<FHeartGraphPinDesc>& Pins) const override;
	//~ IHeartNodeComponentPinProvider

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|InstancedPins")
	uint8 GetInstancedInputNum() const { return InstancedInputs; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|InstancedPins")
	uint8 GetInstancedOutputNum() const { return InstancedOutputs; }

	UFUNCTION(BlueprintCallable, Category = "Heart|InstancedPins")
	bool CanUserAddInput() const { return UserCanAddInputs; }

	UFUNCTION(BlueprintCallable, Category = "Heart|InstancedPins")
	bool CanUserAddOutput() const { return UserCanAddOutputs; }

	void ResetPinCounts();
	FHeartGraphPinDesc MakeInstancedPin(EHeartPinDirection Direction, int32 Index) const;

	// Add a numbered instance pin
	UFUNCTION(BlueprintCallable, Category = "Heart|InstancedPins")
	FHeartPinGuid AddInstancePin(const FHeartNodeGuid& Node, EHeartPinDirection Direction);

	// Remove the last numbered instance pin. Returns the guid of the removed pin, if a pin was removed.
	UFUNCTION(BlueprintCallable, Category = "Heart|InstancedPins")
	FHeartPinGuid RemoveInstancePin(const FHeartNodeGuid& Node, EHeartPinDirection Direction);

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	bool UserCanAddInputs = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	bool UserCanAddOutputs = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Config")
	FHeartGraphPinTag PinTag;

	// Optional metadata objects that add additional info about this pin, such as default values, or connection rules.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Config")
	TArray<TObjectPtr<UHeartGraphPinMetadata>> PinMetadata;

	// Friendlier, localizable name of this pin.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config", AdvancedDisplay)
	FText PinFriendlyName;

	// Runtime tooltip can that can be shown to players.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config", meta = (MultiLine), AdvancedDisplay)
	FText PinTooltip;

#if WITH_EDITORONLY_DATA
	// Additional tooltip only visible in the editor.
	UPROPERTY(EditAnywhere, Category = "Config", AdvancedDisplay)
	FText PinEditorTooltip;
#endif

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Pins")
	uint8 InstancedInputs = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Pins")
	uint8 InstancedOutputs = 0;
};
