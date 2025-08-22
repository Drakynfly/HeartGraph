// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Components/SlateWrapperTypes.h"
#include "Input/HeartEvent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartSlateReplyWrapper.generated.h"

// Enable using wrapped slate Replies as Heart Events
template <> struct Heart::TIsEventType<FEventReply> { static constexpr bool Value = true; };

namespace Heart::Input
{
	// Make a slate-compatible Reply from a HeartEvent
	FReply HeartEventToReply(const FHeartEvent& Event);

	// Make a HeartEvent into a slate-compatible Reply
	FHeartEvent ReplyToHeartEvent(const FHeartEvent& Base, const FReply& Reply);
}

/**
 *
 */
UCLASS()
class HEARTCANVAS_API UHeartSlateReplyWrapper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Heart|SlateReplyWrapper")
	static FEventReply HeartEventToEventReply(const FHeartEvent& Event);

	UFUNCTION(BlueprintPure, Category = "Heart|SlateReplyWrapper")
	static FHeartEvent ReplyEventToHeartEvent(const FHeartEvent& Event, const FEventReply& EventReply);

	UFUNCTION(BlueprintPure, Category = "Heart|SlateReplyWrapper")
	static bool IsAnEventReply(const FHeartEvent& Event);
};