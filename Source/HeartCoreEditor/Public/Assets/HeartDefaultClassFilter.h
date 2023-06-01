// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ClassViewerFilter.h"

class HEARTCOREEDITOR_API FHeartDefaultClassFilter : public IClassViewerFilter
{
public:
	FHeartDefaultClassFilter() {}

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass,
								TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override;

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions,
										const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData,
										TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override;

	/** All children of these classes will be included unless filtered out by another setting. */
	TSet<const UClass*> AllowedChildrenOfClasses;

	/** Disallowed class flags. */
	EClassFlags DisallowedClassFlags = CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_HideDropDown;
};
