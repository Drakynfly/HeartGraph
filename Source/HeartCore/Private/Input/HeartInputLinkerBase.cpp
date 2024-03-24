// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartInputLinkerBase.h"

#include "Input/HeartInputHandlerAssetBase.h"
#include "Input/HeartInputBindingAsset.h"

#include "HeartCorePrivate.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartInputLinkerBase)

DECLARE_CYCLE_STAT(TEXT("HandleOnMouseWheel"),		STAT_HandleOnMouseWheel, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleOnMouseButtonDown"),	STAT_HandleOnMouseButtonDown, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleOnMouseButtonUp"),	STAT_HandleOnMouseButtonUp, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleOnKeyDown"),			STAT_HandleOnKeyDown, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleOnKeyUp"),			STAT_HandleOnKeyUp, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleOnDragDetected"),	STAT_HandleOnDragDetected, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleNativeOnDragOver"),	STAT_HandleNativeOnDragOver, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleNativeOnDrop"),		STAT_HandleNativeOnDrop, STATGROUP_HeartCore);

DECLARE_CYCLE_STAT(TEXT("HandleManualInput"), STAT_HandleManualInput, STATGROUP_HeartCore);

using namespace Heart::Input;

void UHeartInputLinkerBase::BindInputCallback(const FInputTrip& Trip, const FConditionalCallback& InputCallback)
{
	if (ensure(Trip.IsValid()))
	{
		InputCallbackMappings.Add(Trip, InputCallback);
	}
}

void UHeartInputLinkerBase::UnbindInputCallback(const FInputTrip& Trip)
{
	InputCallbackMappings.Remove(Trip);
}

void UHeartInputLinkerBase::AddBindings(const TArray<FHeartBoundInput>& Bindings)
{
	for (auto&& Binding : Bindings)
	{
		if (!IsValid(Binding.InputHandler)) continue;

		Binding.InputHandler->Bind(this, Binding.Triggers);
	}
}

void UHeartInputLinkerBase::RemoveBindings(const TArray<FHeartBoundInput>& Bindings)
{
	for (auto&& Binding : Bindings)
	{
		if (!IsValid(Binding.InputHandler)) continue;

		Binding.InputHandler->Unbind(this, Binding.Triggers);
	}
}