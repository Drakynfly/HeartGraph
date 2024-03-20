// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphWidgetBase.h"
#include "GraphRegistry/HeartNodeSource.h"
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
	void AddNode(FHeartNodeSource NodeSource);

	UFUNCTION(BlueprintCallable, Category = "NodePaletteCategory")
	UHeartNodePalette* GetPalette() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "NodePaletteCategory")
	UWidget* MakeWidgetForNode(FHeartNodeSource NodeSource);
};


class UHeartGraph;
class UHeartRegistryQuery;

/**
 * Base widget class for panels that display a list of nodes available to place in a graph.
 */
UCLASS(Abstract)
class HEARTCANVAS_API UHeartNodePalette : public UHeartGraphWidgetBase
{
	GENERATED_BODY()

	friend UHeartNodePaletteCategory;

public:
	UHeartNodePalette();

protected:
	virtual bool Initialize() override;

	//~ IHeartWidgetInputLinkerRedirector
	virtual UHeartWidgetInputLinker* ResolveLinker_Implementation() const override;
	//~

	virtual void Reset();
	virtual void Display(const TMap<FHeartNodeSource, TSubclassOf<UHeartGraphNode>>& Classes);

	UHeartNodePaletteCategory* FindOrCreateCategory(const FText& Category);

	UUserWidget* CreateNodeWidgetFromFactory(FHeartNodeSource NodeSource);

	UHeartRegistryQuery* GetQuery() const { return Query; }

public:
	/** Regenerate the list of nodes in the palette, triggering the Query to run. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Node Palette")
	void RefreshPalette();

	UFUNCTION(BlueprintCallable, Category = "Heart|Node Palette")
	const FHeartWidgetFactoryRules& GetWidgetFactory() const { return WidgetFactory; }

	UFUNCTION(BlueprintNativeEvent, Category = "Node Palette")
	bool ShouldDisplayNode(const FHeartNodeSource NodeSource, TSubclassOf<UHeartGraphNode> GraphNodeClass);

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

	UPROPERTY(BlueprintReadOnly, Category = "NodePalette")
	TObjectPtr<UHeartRegistryQuery> Query;

	UPROPERTY(BlueprintReadOnly, Category = "NodePalette")
	TMap<FString, TObjectPtr<UHeartNodePaletteCategory>> Categories;
};