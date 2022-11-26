// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "HeartEdGraphSchema.generated.h"

class UHeartGraph;
class UHeartGraphNode;

//DECLARE_MULTICAST_DELEGATE(FHeartGraphSchemaRefresh);

UCLASS()
class HEARTEDITOR_API UHeartEdGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

private:
	//static TArray<UClass*> NativeHeartGraphNodes;
	//static TMap<FName, FAssetData> BlueprintHeartGraphNodes;
	//static TMap<UClass*, UClass*> AssignedGraphNodeClasses;

	//static bool bBlueprintCompilationPending;

public:
	static void SubscribeToAssetChanges();
	static void GetPaletteActions(FGraphActionMenuBuilder& ActionMenuBuilder, const UClass* AssetClass, const FString& CategoryName);

	// EdGraphSchema
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
	virtual bool ShouldHidePinDefaultValue(UEdGraphPin* Pin) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual void BreakNodeLinks(UEdGraphNode& TargetNode) const override;
	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const override;
	virtual int32 GetNodeSelectionCount(const UEdGraph* Graph) const override;
	virtual TSharedPtr<FEdGraphSchemaAction> GetCreateCommentAction() const override;

	// FIX THIS TYPO EPIC !!!
	virtual void OnPinConnectionDoubleCicked(UEdGraphPin* PinA, UEdGraphPin* PinB, const FVector2D& GraphPosition) const override final;
	virtual void OnPinConnectionDoubleClicked(UEdGraphPin* PinA, UEdGraphPin* PinB, const FVector2D& GraphPosition) const {}
	// --

	static TArray<TSharedPtr<FString>> GetHeartGraphNodeCategories(TSubclassOf<UHeartGraph> HeartGraphClass);
	static UClass* GetAssignedEdGraphNodeClass(const UClass* HeartGraphNodeClass);


private:
	static void GetHeartGraphNodeActions(FGraphActionMenuBuilder& ActionMenuBuilder, const UHeartGraph* AssetClassDefaults, const FString& CategoryName);
	static void GetCommentAction(FGraphActionMenuBuilder& ActionMenuBuilder, const UEdGraph* CurrentGraph = nullptr);

	static bool IsHeartGraphNodePlaceable(const UClass* Class);

	//static void OnBlueprintPreCompile(UBlueprint* Blueprint);
	//static void OnBlueprintCompiled();

	//static void GatherHeartGraphNodes();
	//static void OnHotReload(EReloadCompleteReason ReloadCompleteReason);

	//static void OnAssetAdded(const FAssetData& AssetData);
	//static void AddAsset(const FAssetData& AssetData, const bool bBatch);
	//static void OnAssetRemoved(const FAssetData& AssetData);

public:
	//static FHeartGraphSchemaRefresh OnNodeListChanged;
	static UBlueprint* GetPlaceableNodeBlueprint(const FAssetData& AssetData);

	static const UHeartGraph* GetAssetClassDefaults(const UEdGraph* Graph);
};
