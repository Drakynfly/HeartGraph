// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphWidgetBase.h"
#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "UI/HeartWidgetFactory.h"
#include "HeartNodePalette.generated.h"

UCLASS(Abstract)
class UHeartNodePaletteCategory : public UHeartGraphWidgetBase
{
	GENERATED_BODY()

	friend class UHeartNodePalette;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "NodePaletteCategory")
	void SetLabel(const FText& Text);

	UFUNCTION(BlueprintImplementableEvent, Category = "NodePaletteCategory")
	void ClearChildren();

	UFUNCTION(BlueprintImplementableEvent, Category = "NodePaletteCategory")
	void AddNode(UClass* NodeClass);

	UFUNCTION(BlueprintCallable, Category = "NodePaletteCategory")
	UHeartNodePalette* GetPalette() const;

	UFUNCTION(BlueprintNativeEvent, Category = "NodePaletteCategory")
	UWidget* MakeWidgetForNode(UClass* NodeClass);
};


class UHeartGraph;

/**
 * Base widget class for panels that display a list of nodes available to place in a graph.
 */
UCLASS(Abstract)
class HEARTCANVAS_API UHeartNodePalette : public UHeartGraphWidgetBase
{
	GENERATED_BODY()

	friend UHeartNodePaletteCategory;

protected:
	virtual bool Initialize() override;

	//~ IHeartWidgetInputLinkerRedirector
	virtual UHeartWidgetInputLinker* ResolveLinker_Implementation() const override;
	//~

	virtual void Reset();
	virtual void Display(const TMap<UClass*, TSubclassOf<UHeartGraphNode>>& Classes);

	UHeartNodePaletteCategory* FindOrCreateCategory(const FText& Category);

	UUserWidget* CreateNodeWidgetFromFactory(UClass* NodeClass);

public:
	/** Regenerate the list of nodes in the palette, triggering the filter for each node again. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Node Palette")
	void RefreshPalette();

	/** Set a custom filter function. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Node Palette")
	void SetFilter(const FNodeClassFilter& NewFilter, bool bRefreshPalette);

	UFUNCTION(BlueprintCallable, Category = "Heart|Node Palette")
	void ClearFilter(bool bRefreshPalette);

	UFUNCTION(BlueprintCallable, Category = "Heart|Node Palette")
	const FHeartWidgetFactoryRules& GetWidgetFactory() const { return WidgetFactory; }

	UFUNCTION(BlueprintNativeEvent, Category = "Node Palette")
	bool ShouldDisplayNode(const UClass* NodeClass, TSubclassOf<UHeartGraphNode> GraphNodeClass);

	/** */
	UFUNCTION(BlueprintImplementableEvent, Category = "Node Palette")
	void OnReset();

	/** */
	UFUNCTION(BlueprintImplementableEvent, Category = "Node Palette")
	void OnDisplay();

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> PalettePanel;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NodePalette")
	TSubclassOf<UHeartGraph> DisplayedRegistryGraph;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NodePalette")
	TSubclassOf<UHeartNodePaletteCategory> CategoryClass;

	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NodePalette")
	FHeartWidgetFactoryRules WidgetFactory;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (ShowOnlyInnerProperties))
	FHeartWidgetInputBindingContainer BindingContainer;

	UPROPERTY(EditAnywhere, Category = "Events", meta = (IsBindableEvent = "True"))
	FNodeClassFilter Filter;

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, TObjectPtr<UHeartNodePaletteCategory>> Categories;
};