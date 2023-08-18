// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "EditorSubsystem.h"
#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "HeartRegistryEditorSubsystem.generated.h"

class UHeartEdGraphNode;

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
	/**
	 * Gets the list of classes that should show up in the Common Classes section of the Class Picker Dialog when
	 * creating a new Heart Graph Asset
	 */
	static TArray<UClass*> GetFactoryCommonClasses();

	/**
	 * Creates a slate widget for a EdGraphNode, by looking up a style registered with the Module.
	 */
	TSharedPtr<SGraphNode> MakeVisualWidget(FName Style, UHeartEdGraphNode* Node) const;

	/**
	 * Get the class of UEdGraphNode to represent a given class of UHeartGraphNode
	 */
	UClass* GetAssignedEdGraphNodeClass(const TSubclassOf<UHeartGraphNode> HeartGraphNodeClass) const;

protected:
	void FetchAssetRegistryAssets();

	bool BlueprintImplementsHeartVisualizerInterface(const UBlueprint* Blueprint) const;

	void OnHotReload(EReloadCompleteReason ReloadCompleteReason);
	void OnBlueprintPreCompile(UBlueprint* Blueprint);
	void OnBlueprintCompiled(UBlueprint* Blueprint);

	void PreRegistryAdded(UHeartGraphNodeRegistry* HeartGraphNodeRegistry);
	void PostRegistryRemoved(UHeartGraphNodeRegistry* HeartGraphNodeRegistry);
	void OnAnyRegistryChanged(UHeartGraphNodeRegistry* HeartGraphNodeRegistry);

public:
	using FHeartRegistryEditorPaletteRefresh = TMulticastDelegate<void()>;
	FHeartRegistryEditorPaletteRefresh OnRefreshPalettes;

private:
	TSet<TWeakObjectPtr<UBlueprint>> WaitingForCompile;
};