// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartPlayerInput.h"
#include "Input/HeartInputLinkerBase.h"
#include "Input/HeartInputLinkerInterface.h"
#include "Input/HeartSceneInputLinker.h"

bool UHeartPlayerInput::InputKey(const FInputKeyParams& Params)
{
	class AStupidHackPlayerController : APlayerController
	{
		friend UHeartPlayerInput; // :)
	};

	bool bResult = Super::InputKey(Params);

	UObject* Target = reinterpret_cast<AStupidHackPlayerController*>
		(GetOuterAPlayerController())->CurrentClickablePrimitive.Get();

	if (IsValid(Target))
	{
		if (UHeartSceneInputLinker* Linker = Heart::Input::TryFindLinker<UHeartSceneInputLinker>(Target);
			IsValid(Linker))
		{
			if (Linker->InputKey(Params, Target))
			{
				bResult = true;
			}
		}
	}

	return bResult;
}