// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "HeartNodePalette.generated.h"

class UHeartGraph;

/**
 * Base widget class for panels that display a list of nodes available to place in a graph.
 */
UCLASS(Abstract)
class HEART_API UHeartNodePalette : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void Reset();
	virtual void Display(const TArray<UClass*>& Classes);

public:
	/** Regenerate the list of nodes in the palette, triggering the filter for each node again. */
	UFUNCTION(BlueprintCallable, Category = "Node Palette")
	void RefreshPalette();

	/** Set a custom filter function. */
	UFUNCTION(BlueprintCallable, Category = "Node Palette")
	void SetFilter(const FNodeClassFilter& NewFilter, bool bRefreshPalette);

	UFUNCTION(BlueprintCallable, Category = "Node Palette")
	void ClearFilter(bool bRefreshPalette);

	/** */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Reset"))
	void BP_Reset();

	/** */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Display"))
	void BP_Display(const TArray<UClass*>& Classes);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heart|NodePalette")
	TSubclassOf<UHeartGraph> DisplayedRegistryGraph;

	UPROPERTY()
	FNodeClassFilter Filter;
};
