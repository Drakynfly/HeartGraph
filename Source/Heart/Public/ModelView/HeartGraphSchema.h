// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGraphSchema.generated.h"

class UHeartGraphPin;
class UHeartGraphNode;

/** This is the type of response the graph editor should take when making a connection */
UENUM(BlueprintType)
enum class EHeartCanConnectPinsResponse : uint8
{
	/** Make the connection; there are no issues (message string is displayed if not empty). */
	Allow,

	/** Cannot make this connection; display the message string as an error. */
	Disallow,

	/** Break all existing connections on A and make the new connection (it's exclusive); display the message string as a warning/notice. */
	AllowBreakA,

	/** Break all existing connections on B and make the new connection (it's exclusive); display the message string as a warning/notice. */
	AllowBreakB,

	/** Break all existing connections on A and B, and make the new connection (it's exclusive); display the message string as a warning/notice. */
	AllowBreakAB,

	//** Make the connection via an intermediate cast node, or some other conversion node. */
	//CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE,

	/** Make the connection by promoting a lower type to a higher type. Ex: Connecting a Float -> Double, float should become a double */
	//CONNECT_RESPONSE_MAKE_WITH_PROMOTION,

	//CONNECT_RESPONSE_MAX,
};

USTRUCT(BlueprintType)
struct FHeartConnectPinsResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartConnectPinsResponse")
	EHeartCanConnectPinsResponse Response;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartConnectPinsResponse")
	FText Message;
};

UCLASS(Abstract, Const, BlueprintType, Blueprintable)
class HEART_API UHeartGraphSchema : public UObject // UEdGraphSchema
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure = false, BlueprintNativeEvent, Category = "Heart|Schema")
	bool TryConnectPins(UHeartGraphPin* PinA, UHeartGraphPin* PinB) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|Schema")
	FHeartConnectPinsResponse CanPinsConnect(UHeartGraphPin* PinA, UHeartGraphPin* PinB) const;
};
