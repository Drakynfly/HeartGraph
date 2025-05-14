// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartPlayerInput.h"
#include "Input/HeartInputLinkerBase.h"
#include "Input/HeartSceneInputLinker.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartPlayerInput)

bool UHeartPlayerInput::InputKey(const FInputKeyEventArgs& Params)
{
	class AStupidHackPlayerController : APlayerController
	{
		friend UHeartPlayerInput; // :)
	};

	bool bResult = Super::InputKey(Params);

	UPrimitiveComponent* Target = reinterpret_cast<AStupidHackPlayerController*>
		(GetOuterAPlayerController())->CurrentClickablePrimitive.Get();

	if (IsValid(Target))
	{
		bResult = Heart::Input::InvokeLinker<USceneComponent, bool>(Target, &UHeartSceneInputLinker::InputKey, Params);
	}

	return bResult;
}