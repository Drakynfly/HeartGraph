// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "SGraphPin.h"

class HEARTEDITOR_API SHeartGraphPin : public SGraphPin
{
public:
	SHeartGraphPin();

	SLATE_BEGIN_ARGS(SHeartGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
};