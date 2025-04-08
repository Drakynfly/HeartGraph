// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "HeartEdGraphSchema.generated.h"

class UHeartGraph;
class UHeartGraphNode;

UCLASS()
class HEARTEDITOR_API UHeartEdGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	static void GetPaletteActions(FGraphActionMenuBuilder& ActionMenuBuilder, const UHeartGraph* GraphAsset, const TOptional<FStringView>& CategoryName);

	//~ EdGraphSchema
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const override;
	virtual bool TryCreateConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const override;
	virtual bool ShouldHidePinDefaultValue(UEdGraphPin* Pin) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual void BreakNodeLinks(UEdGraphNode& TargetNode) const override;
	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const override;
	virtual int32 GetNodeSelectionCount(const UEdGraph* Graph) const override;
	virtual TSharedPtr<FEdGraphSchemaAction> GetCreateCommentAction() const override;
	virtual FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const override;

	// FIX THIS TYPO EPIC !!!
	virtual void OnPinConnectionDoubleCicked(UEdGraphPin* PinA, UEdGraphPin* PinB, const FVector2D& GraphPosition) const override final;
	virtual void OnPinConnectionDoubleClicked(UEdGraphPin* PinA, UEdGraphPin* PinB, const FVector2D& GraphPosition) const;
	// --

	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	//~ EdGraphSchema

	static TArray<TSharedPtr<FString>> GetHeartGraphNodeCategories(const UHeartGraph* GraphAsset);

private:
	static void GetHeartGraphNodeActions(FGraphActionMenuBuilder& ActionMenuBuilder, const UHeartGraph* GraphAsset, const TOptional<FStringView>& CategoryName);
	static void GetCommentAction(FGraphActionMenuBuilder& ActionMenuBuilder, const UEdGraph* CurrentGraph = nullptr);

public:
	static const UHeartGraph* GetAssetFromEdGraph(const UEdGraph* Graph);
	static const UHeartGraph* GetAssetClassDefaults(const UEdGraph* Graph);
};