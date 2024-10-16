// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphPinTag.h"
#include "HeartPinDirection.h"
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartGraphPinDesc")
	FName Name;

	// Tag used to identify the type of pin this represents.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartGraphPinDesc")
	FHeartGraphPinTag Tag;

	// Direction of this pin, input, or output.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartGraphPinDesc")
	EHeartPinDirection Direction = EHeartPinDirection::Input;

	// Optional metadata objects that add additional info about this pin, such as default values, or connection rules.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "HeartGraphPinDesc")
	TArray<TObjectPtr<UHeartGraphPinMetadata>> Metadata;

	// Friendlier, localizable name of this pin.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartGraphPinDesc", AdvancedDisplay)
	FText FriendlyName;

	// Runtime tooltip can that can be shown to players.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartGraphPinDesc", meta = (MultiLine), AdvancedDisplay)
	FText Tooltip;

#if WITH_EDITORONLY_DATA
	// Additional tooltip only visible in the editor.
	UPROPERTY(EditAnywhere, Category = "HeartGraphPinDesc", AdvancedDisplay)
	FText EditorTooltip;
#endif

	bool IsValid() const
	{
		return !Name.IsNone() && Tag.IsValid() && Direction != EHeartPinDirection::None;
	}
};

namespace Heart::Graph
{
	static const FHeartGraphPinDesc InvalidPinDesc{NAME_None, {}, EHeartPinDirection::None};
}