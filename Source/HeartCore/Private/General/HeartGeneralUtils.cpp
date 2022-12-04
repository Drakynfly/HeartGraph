// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "General/HeartGeneralUtils.h"
#include "General/VectorBounds.h"
#include "General/Vector2DBounds.h"

UObject* UHeartGeneralUtils::K2_DuplicateObject(UObject* Outer, UObject* Source)
{
	return DuplicateObject(Source, Outer);
}

UClass* UHeartGeneralUtils::GetParentClass(const UClass* Class)
{
	return IsValid(Class) ? Class->GetSuperClass() : nullptr;
}

TArray<UClass*> UHeartGeneralUtils::GetChildClasses(const UClass* Class, const bool AllowAbstract)
{
	TArray<UClass*> OutClasses;

	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		if (!(*ClassIt)->IsChildOf(Class) || ((*ClassIt)->HasAnyClassFlags(CLASS_Abstract | CLASS_NewerVersionExists) || AllowAbstract))
		{
			continue;
		}

		OutClasses.Add(*ClassIt);
	}

	return OutClasses;
}

UObject* UHeartGeneralUtils::ClassAsObject(UClass* Class)
{
	return Class;
}

const UObject* UHeartGeneralUtils::GetClassDefaultObject(UClass* Class)
{
	return IsValid(Class) ? Class->GetDefaultObject() : nullptr;
}

FText UHeartGeneralUtils::GetClassDisplayNameText(const UClass* Class)
{
	return IsValid(Class) ? Class->GetDisplayNameText() : FText();
}

FText UHeartGeneralUtils::GetClassTooltip(const UClass* Class)
{
	return IsValid(Class) ? Class->GetToolTipText() : FText();
}

FVector UHeartGeneralUtils::BP_ClampVector(const FVector& Value, const FVectorBounds& Bounds)
{
	return ClampVector(Value, Bounds.Min, Bounds.Max);
}

FVector2D UHeartGeneralUtils::BP_ClampVector2D(const FVector2D& Value, const FVector2DBounds& Bounds)
{
	return FVector2D(FMath::Clamp(Value.X, Bounds.Min.X, Bounds.Max.X),
					 FMath::Clamp(Value.Y, Bounds.Min.Y, Bounds.Max.Y));
}

FVector2D UHeartGeneralUtils::ComputeSplineTangent(const FVector2D& Start, const FVector2D& End, const float Direction,
                                                   const float TensionMultiplier)
{
	auto&& RotatedStartAngle = Start.GetRotated(Direction).Y;
	auto&& RotatedEndAngle = End.GetRotated(Direction).Y;
	auto&& ReversedSpline = (RotatedStartAngle - RotatedEndAngle) < 0.f;
	auto&& TensionScalar = RotatedStartAngle - RotatedEndAngle;
	auto&& TensionVector = FVector2D(0, ReversedSpline ? -TensionMultiplier : TensionMultiplier);
	return FVector2D(TensionScalar) * TensionVector.GetRotated(Direction);
}
