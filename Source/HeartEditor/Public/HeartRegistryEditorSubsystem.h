// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "EditorSubsystem.h"
#include "HeartRegistryEditorSubsystem.generated.h"

/**
 *
 */
UCLASS()
class HEARTEDITOR_API UHeartRegistryEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	/*
	 * Gets the list of classes that should show up in the Common Classes section of the Class Picker Dialog when
	 * creating a new Heart Graph Asset
	 */
	static TArray<UClass*> GetFactoryCommonClasses();

protected:
	void SubscribeToAssetChanges();
	void FetchAssetRegistryAssets();

	void OnFilesLoaded();
	void OnAssetAdded(const FAssetData& AssetData);
	void OnAssetRemoved(const FAssetData& AssetData);
	void OnHotReload(EReloadCompleteReason ReloadCompleteReason);
	void OnBlueprintPreCompile(UBlueprint* Blueprint);
	void OnBlueprintCompiled();


	UBlueprint* GetNodeBlueprint(const FAssetData& AssetData) const;

private:
	int32 WaitingForBlueprintToCompile;
};
