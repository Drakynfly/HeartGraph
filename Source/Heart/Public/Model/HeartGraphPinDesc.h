// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphPinTag.h"
#include "HeartGraphPinDesc.generated.h"

enum class EHeartPinDirection : uint8;
class UHeartGraphPinMetadata;

/**
 * Complete description of a Heart Graph Pin.
 */
USTRUCT(BlueprintType)
struct FHeartGraphPinDesc
{
	GENERATED_BODY()

	// Internal ID for this pin, not necessarily user-facing.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Heart|PinDesc")
	FName Name;

	// Friendlier, localizable name of this pin.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Heart|PinDesc")
	FText FriendlyName;

	// Runtime tooltip can that can be shown to players.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Heart|PinDesc", meta = (MultiLine))
	FText Tooltip;

#if WITH_EDITORONLY_DATA
	// Additional tooltip only visible in the editor.
	UPROPERTY(EditAnywhere, Category = "Heart|PinDesc")
	FText EditorTooltip;
#endif

	// Tag used to identify the type of pin this represents.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Heart|PinDesc")
	FHeartGraphPinTag Tag;

	// Direction of this pin, input, or output.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Heart|PinDesc")
	EHeartPinDirection Direction;

	// Optional metadata objects that add additional info about this pin, such as default values, or connection rules.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Heart|PinDesc")
	TArray<TObjectPtr<UHeartGraphPinMetadata>> Metadata;

	bool IsValid() const
	{
		return !Name.IsNone() && Tag.IsValid();
	}
};