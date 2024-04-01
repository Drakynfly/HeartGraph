// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartEvent.h"
#include "HeartInputActivation.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartInputUtilsLibrary.generated.h"

struct FHeartInputActivation;
class UHeartInputBindingAsset;
class UHeartInputLinkerBase;

namespace Heart::Input
{
	// Stolen from UWidgetInteractionComponent
	static void GetKeyAndCharCodes(const FKey& Key, bool& bHasKeyCode, uint32& KeyCode, bool& bHasCharCode, uint32& CharCode)
	{
		const uint32* KeyCodePtr;
		const uint32* CharCodePtr;
		FInputKeyManager::Get().GetCodesFromKey(Key, KeyCodePtr, CharCodePtr);

		bHasKeyCode = KeyCodePtr ? true : false;
		bHasCharCode = CharCodePtr ? true : false;

		KeyCode = KeyCodePtr ? *KeyCodePtr : 0;
		CharCode = CharCodePtr ? *CharCodePtr : 0;

		// These special keys are not handled by the platform layer, and while not printable
		// have character mappings that several widgets look for, since the hardware sends them.
		if (CharCodePtr == nullptr)
		{
			if (Key == EKeys::Tab)
			{
				CharCode = '\t';
				bHasCharCode = true;
			}
			else if (Key == EKeys::BackSpace)
			{
				CharCode = '\b';
				bHasCharCode = true;
			}
			else if (Key == EKeys::Enter)
			{
				CharCode = '\n';
				bHasCharCode = true;
			}
		}
	}

	static FKeyEvent MakeKeyEventFromKey(const FKey& Key)
	{
		bool bHasKeyCode, bHasCharCode;
		uint32 KeyCode, CharCode;
		GetKeyAndCharCodes(Key, bHasKeyCode, KeyCode, bHasCharCode, CharCode);

		return FKeyEvent(Key, FModifierKeysState(), 0, false, KeyCode, CharCode);
	}
}

/**
 *
 */
UCLASS()
class HEARTCORE_API UHeartInputUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// @todo move to math lib?
	UFUNCTION(BlueprintPure, Category = "Heart|WidgetUtilsLibrary")
	static int32 FindClosestToDirection(const TArray<FVector2D>& Locations, FVector2D From, FVector2D Direction, float DotRange = 0.5);

	// Try to find the Input Linker for a widget.
	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	static UHeartInputLinkerBase* GetInputLinker(const UObject* Target);

	/**
	 * Get all manually-keyed actions for a widget. This only works if either the widget, or one of its parents,
	 * implements IHeartInputLinkerInterface.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	static TArray<struct FHeartManualInputQueryResult> GetInputLinkerActions(const UObject* Target);

	//UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	//static FHeartWidgetInputBinding MakeInputBinding(UHeartInputHandlerAsset* InputHandler, UHeartWidgetInputTrigger* Trigger);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	static bool AddInputBindingAssetToLinker(UHeartInputLinkerBase* Linker, UHeartInputBindingAsset* BindingAsset);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	static bool RemoveInputBindingAssetFromLinker(UHeartInputLinkerBase* Linker, UHeartInputBindingAsset* BindingAsset);

	// Trigger a manual keyed input binding on a linker, if one can be found for the target.
	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetInputLinker")
	static FHeartEvent TriggerManualInput(UObject* Target, FName Key, const FHeartManualEvent& Activation);
};