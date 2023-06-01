// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Assets/HeartDefaultClassFilter.h"

bool FHeartDefaultClassFilter::IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions,
                                              const UClass* InClass,
                                              const TSharedRef<FClassViewerFilterFuncs> InFilterFuncs)
{
	bool Passes = InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;

	if (Passes)
	{
		Passes = !InClass->HasAnyClassFlags(DisallowedClassFlags);
	}

	return Passes;
}

bool FHeartDefaultClassFilter::IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions,
                                                      const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData,
                                                      const TSharedRef<FClassViewerFilterFuncs> InFilterFuncs)
{
	bool Passes = InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;

	if (Passes)
	{
		Passes = !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags);
	}

	return Passes;
}
