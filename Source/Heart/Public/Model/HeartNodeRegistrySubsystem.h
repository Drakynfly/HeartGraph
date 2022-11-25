// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "HeartGraph.h"
#include "HeartRegistrationClasses.h"
#include "HeartNodeRegistrySubsystem.generated.h"

class UHeartGraphNode;
class UHeartGraphPin;
class UGraphNodeRegistrar;

DECLARE_LOG_CATEGORY_EXTERN(LogHeartNodeRegistry, Log, All);

DECLARE_DELEGATE_RetVal_OneParam(bool, FNativeNodeClassFilter, UClass* /* Class*/);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FNodeClassFilter, UClass*, Class);

/**
 * Stores a list of nodes, graph nodes, usable by a Graph, along with their internal graph representations and
 * visual representations.
 */
UCLASS()
class HEART_API UHeartGraphNodeRegistry : public UObject
{
	GENERATED_BODY()

	friend class UHeartNodeRegistrySubsystem;

public:
#if WITH_EDITOR
	void NotifyNodeBlueprintNodeClassAdded(TSubclassOf<UHeartGraphNode> GraphNodeClass);
#endif

protected:
	void AddRegistrationList(const FHeartRegistrationClasses& Registration);
	void RemoveRegistrationList(const FHeartRegistrationClasses& Registration);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	TArray<FString> GetNodeCategories() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	void GetNodeClasses(TArray<UClass*>& OutClasses) const { OutClasses = NodeClasses.Array(); }

	void GetFilteredNodeClasses(const FNativeNodeClassFilter& Filter, TArray<UClass*>& OutClasses) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	void GetFilteredNodeClasses(const FNodeClassFilter& Filter, TArray<UClass*>& OutClasses) const;

	/**
	 * Get the graph node class that we use to represent the given arbitrary class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	UClass* GetGraphNodeClassForNode(UClass* NodeClass) const;

	/**
	 * Get the visualizer class that we use to represent the given node class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	UClass* GetVisualizerClassForGraphNode(TSubclassOf<UHeartGraphNode> GraphNodeClass) const;

	/**
	* Get the visualizer class that we use to represent the given pin class.
	*/
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	UClass* GetVisualizerClassForGraphPin(TSubclassOf<UHeartGraphPin> GraphPinClass) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	void AddRegistrar(UGraphNodeRegistrar* Registrar);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	void RemoveRegistrar(UGraphNodeRegistrar* Registrar);

private:
	UPROPERTY()
	TSet<TObjectPtr<UClass>> NodeClasses;

	// Maps classes to the Graph Node class that can represent them in a graph.
	UPROPERTY()
	TMap<TObjectPtr<UClass>, TSubclassOf<UHeartGraphNode>> GraphNodeMap;

	// Maps Graph Node classes to the visualizer class that can represent them in an interactive graph.
	UPROPERTY()
	TMap<TSubclassOf<UHeartGraphNode>, TObjectPtr<UClass>> NodeVisualizerMap;

	// Maps Graph Pin classes to the visualizer class that can represent them in an interactive graph.
	UPROPERTY()
	TMap<TSubclassOf<UHeartGraphPin>, TObjectPtr<UClass>> PinVisualizerMap;

	UPROPERTY()
	TArray<TObjectPtr<UGraphNodeRegistrar>> ContainedRegistrars;
};

/**
 * Global singleton that stores which graphs can use which registries.
 */
UCLASS()
class HEART_API UHeartNodeRegistrySubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

#if WITH_EDITOR
	void OnFilesLoaded();
	void OnAssetAdded(const FAssetData& AssetData);
	void OnAssetRemoved(const FAssetData& AssetData);
	void OnHotReload(EReloadCompleteReason ReloadCompleteReason);
	void OnBlueprintPreCompile(UBlueprint* Blueprint);
	void OnBlueprintCompiled();
#endif

protected:
	UBlueprint* GetNodeBlueprint(const FAssetData& AssetData);

	void FetchNativeClasses();
	void FetchAssetRegistryAssets();

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|NodeRegistrySubsystem")
	UHeartGraphNodeRegistry* GetRegistry(const TSubclassOf<UHeartGraph> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|NodeRegistrySubsystem")
	void AddRegistrar(UGraphNodeRegistrar* Registrar);

	UFUNCTION(BlueprintCallable, Category = "Heart|NodeRegistrySubsystem")
	void RemoveRegistrar(UGraphNodeRegistrar* Registrar);

private:
	UPROPERTY()
	TMap<TSubclassOf<UHeartGraph>, TObjectPtr<UHeartGraphNodeRegistry>> NodeRegistries;

#if WITH_EDITOR
	int32 WaitingForBlueprintToCompile;
	FHeartRegistrationClasses KnownNativeClasses;
	TMap<FName, FAssetData> KnownBlueprintHeartGraphNodes;
#endif
};
