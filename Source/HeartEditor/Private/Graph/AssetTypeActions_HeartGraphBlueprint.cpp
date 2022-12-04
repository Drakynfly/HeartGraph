// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/AssetTypeActions_HeartGraphBlueprint.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphBlueprint.h"
#include "Graph/HeartGraphBlueprintFactory.h"

#include "HeartEditorModule.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_HeartGraphBlueprint"

FText FAssetTypeActions_HeartGraphBlueprint::GetName() const
{
	return LOCTEXT("AssetTypeActions_HeartGraphBlueprint", "Heart Graph Blueprint");
}

uint32 FAssetTypeActions_HeartGraphBlueprint::GetCategories()
{
	return FHeartEditorModule::HeartAssetCategory;
}

UClass* FAssetTypeActions_HeartGraphBlueprint::GetSupportedClass() const
{
	return UHeartGraphBlueprint::StaticClass();
}

UFactory* FAssetTypeActions_HeartGraphBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	auto&& HeartGraphNodeBlueprintFactory = NewObject<UHeartGraphBlueprintFactory>();
	HeartGraphNodeBlueprintFactory->ParentClass = TSubclassOf<UHeartGraph>(*InBlueprint->GeneratedClass);
	return HeartGraphNodeBlueprintFactory;
}

#undef LOCTEXT_NAMESPACE
