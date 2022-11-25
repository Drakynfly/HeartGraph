// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartGraphFactory.h"
#include "Model/HeartGraph.h"
#include "Graph/HeartEdGraph.h"

#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/SClassPickerDialog.h"

#define LOCTEXT_NAMESPACE "HeartAssetFactory"

class FAssetClassParentFilter : public IClassViewerFilter
{
public:
	FAssetClassParentFilter()
		: DisallowedClassFlags(CLASS_None)
	{
	}

	/** All children of these classes will be included unless filtered out by another setting. */
	TSet<const UClass*> AllowedChildrenOfClasses;

	/** Disallowed class flags. */
	EClassFlags DisallowedClassFlags;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
	{
		const bool bAllowed = !InClass->HasAnyClassFlags(DisallowedClassFlags) && InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;

		if (bAllowed)
		{
			if (!FKismetEditorUtilities::CanCreateBlueprintOfClass(InClass))
			{
				return false;
			}

			// @todo this doesnt seem to work
			if (!InClass->GetDefaultObject<UHeartGraph>()->GetCanCreateAssetFromFactory())
			{
				return false;
			}
		}

		return bAllowed;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
	{
		return !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags) && InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
	}
};

UHeartGraphFactory::UHeartGraphFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UHeartGraph::StaticClass();

	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

bool UHeartGraphFactory::ConfigureProperties()
{
	// Load the Class Viewer module to display a class picker
	FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	// Fill in options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;

	const TSharedPtr<FAssetClassParentFilter> Filter = MakeShareable(new FAssetClassParentFilter);
	Filter->DisallowedClassFlags = CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_HideDropDown;
	Filter->AllowedChildrenOfClasses.Add(UHeartGraph::StaticClass());

	Options.ClassFilters = {Filter.ToSharedRef()};

	const FText TitleText = LOCTEXT("CreateHeartAssetOptions", "Pick Heart Graph Class");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UHeartGraph::StaticClass());

	if (bPressedOk)
	{
		AssetClass = ChosenClass;
	}

	return bPressedOk;
}

UObject* UHeartGraphFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UHeartGraph* NewHeartGraph = nullptr;
	if (AssetClass != nullptr)
	{
		NewHeartGraph = NewObject<UHeartGraph>(InParent, AssetClass, Name, Flags | RF_Transactional, Context);
	}
	else
	{
		// if we have no asset class, use the passed-in class instead
		NewHeartGraph = NewObject<UHeartGraph>(InParent, Class, Name, Flags | RF_Transactional, Context);
	}

	UHeartEdGraph::CreateGraph(NewHeartGraph);
	return NewHeartGraph;
}

#undef LOCTEXT_NAMESPACE