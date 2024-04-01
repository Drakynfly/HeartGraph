// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Components/SlateWrapperTypes.h"
#include "Input/HeartEvent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartSlateReplyWrapper.generated.h"

// Enable using wrapped slate Replies as Heart Events
template <> struct TIsHeartEventType<FEventReply> { static constexpr bool Value = true; };

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