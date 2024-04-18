// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartInputUtilsLibrary.h"
#include "Input/HeartInputBindingAsset.h"
#include "Input/HeartInputLinkerBase.h"
#include "Input/HeartInputLinkerInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartInputUtilsLibrary)

int32 UHeartInputUtilsLibrary::FindClosestToDirection(const TArray<FVector2D>& Locations, const FVector2D From,
													   const FVector2D Direction, const float DotRange)
{
	int32 Closest = INDEX_NONE;
	double BestScore = 0;

	for (int32 i = 0; i < Locations.Num(); ++i)
	{
		FVector2D Offset = From - Locations[i];

		// Angle between direction and target
		const double Dot = Offset.GetSafeNormal() | Direction;

		if (Dot > DotRange)
		{
			const double Distance = FVector2D::Distance(From, Locations[i]);

			if (Distance > BestScore)
			{
				BestScore = Distance;
				Closest = i;
			}
		}
	}

	return Closest;
}

UHeartInputLinkerBase* UHeartInputUtilsLibrary::GetInputLinker(const UObject* Target)
{
	return Heart::Input::TryFindLinker<UHeartInputLinkerBase>(Target);
}

TArray<FHeartManualInputQueryResult> UHeartInputUtilsLibrary::GetInputLinkerActions(const UObject* Target)
{
	TArray<FHeartManualInputQueryResult> ActionList;

	if (auto&& Linker = Heart::Input::TryFindLinker<UHeartInputLinkerBase>(Target);
		IsValid(Linker))
	{
		ActionList = Linker->QueryManualTriggers(Target);
	}

	return ActionList;
}

bool UHeartInputUtilsLibrary::AddInputBindingAssetToLinker(UHeartInputLinkerBase* Linker, UHeartInputBindingAsset* BindingAsset)
{
	if (!IsValid(Linker) || IsValid(BindingAsset))
	{
		return false;
	}

	Linker->AddBindings(BindingAsset->BindingData);
	return true;
}

bool UHeartInputUtilsLibrary::RemoveInputBindingAssetFromLinker(UHeartInputLinkerBase* Linker, UHeartInputBindingAsset* BindingAsset)
{
	if (!IsValid(Linker) || IsValid(BindingAsset))
	{
		return false;
	}

	Linker->RemoveBindings(BindingAsset->BindingData);
	return true;
}

FHeartEvent UHeartInputUtilsLibrary::TriggerManualInput(UObject* Target, const FName Key, const FHeartManualEvent& Activation)
{
	if (auto&& Linker = Heart::Input::TryFindLinker<UHeartInputLinkerBase>(Target);
		IsValid(Linker))
	{
		return Linker->HandleManualInput(Target, Key, Activation);
	}

	return FHeartEvent::Invalid;
}