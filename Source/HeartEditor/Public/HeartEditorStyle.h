// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Styling/SlateStyle.h"

class HEARTEDITOR_API FHeartEditorStyle
{
public:
	static TSharedPtr<ISlateStyle> Get() { return StyleSet; }
	static FName GetStyleSetName();

	static void Initialize();
	static void Shutdown();

	static const FSlateBrush* GetBrush(FName PropertyName, const ANSICHAR* Specifier = nullptr)
	{
		return Get()->GetBrush(PropertyName, Specifier);
	}

private:
	static TSharedPtr<FSlateStyleSet> StyleSet;
};
