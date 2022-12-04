// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphWidgetBase.h"
#include "Blueprint/UserWidget.h"
#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "UI/HeartWidgetFactory.h"
#include "HeartNodePalette.generated.h"

class UHeartGraph;

/**
 * Base widget class for panels that display a list of nodes available to place in a graph.
 */
UCLASS(Abstract)
class HEARTCANVAS_API UHeartNodePalette : public UHeartGraphWidgetBase
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;

	virtual void Reset();
	virtual void Display(const TArray<UClass*>& Classes);

	virtual UHeartWidgetInputLinker* ResolveLinker_Implementation() const override;

public:
	/** Regenerate the list of nodes in the palette, triggering the filter for each node again. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Node Palette")
	void RefreshPalette();

	/** Set a custom filter function. */
	UFUNCTION(BlueprintCallable, Category = "Heart|Node Palette")
	void SetFilter(const FNodeClassFilter& NewFilter, bool bRefreshPalette);

	UFUNCTION(BlueprintCallable, Category = "Heart|Node Palette")
	void ClearFilter(bool bRefreshPalette);

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

	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NodePalette")
	FHeartWidgetFactoryRules Rules;

	UPROPERTY(EditAnywhere, Category = "Input")
	FHeartWidgetInputBindingContainer BindingContainer;

	UPROPERTY(EditAnywhere, Category = "Events", meta = (IsBindableEvent = "True"))
	FNodeClassFilter Filter;
};
