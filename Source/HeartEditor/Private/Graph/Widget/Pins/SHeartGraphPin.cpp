// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/Widgets/Pins/SHeartGraphPin.h"

SHeartGraphPin::SHeartGraphPin()
{
}

void SHeartGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InPin);

	// @todo expose setting this from the schema
	//bUsePinColorForText = true;
}