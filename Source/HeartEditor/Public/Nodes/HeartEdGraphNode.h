// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "HeartBreakpoint.h"
#include "Model/HeartEdNodeInterface.h"
#include "Model/HeartGraphPinDesc.h"

#include "HeartEdGraphNode.generated.h"

class UEdGraphSchema;
class UHeartGraphNode;

/**
 * Graph representation of a Heart Node in an EdGraph
 */
UCLASS()
class HEARTEDITOR_API UHeartEdGraphNode : public UEdGraphNode, public Heart::IEdNodeInterface
{
	GENERATED_BODY()

public:
	UHeartEdGraphNode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


// Heart Graph Node

	void SetHeartGraphNode(UHeartGraphNode* InHeartGraphNode);
	UHeartGraphNode* GetHeartGraphNode() const;

	// UObject
	virtual void PostLoad() override;
	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
	virtual void PostEditImport() override;
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
	// --

	// UEdGraphNode
	virtual void PostPlacedNewNode() override;
	virtual void PrepareForCopying() override;
	virtual void PostPasteNode() override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
    // --

private:
	// Heart::IEdNodeInterface
	virtual void OnPropertyChanged() override;
	// --

public:
	void PostCopyNode();

private:
	void SubscribeToExternalChanges();

	void OnBlueprintPreCompile(UBlueprint* Blueprint);
	void OnBlueprintCompiled();

	UFUNCTION()
	void OnNodeRequestReconstruction();


// Graph node

public:
	// UEdGraphNode
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	// --

	/**
	 * Handles inserting the node between the FromPin and what the FromPin was original connected to
	 *
	 * @param FromPin			The pin this node is being spawned from
	 * @param NewLinkPin		The new pin the FromPin will connect to
	 * @param OutNodeList		Any nodes that are modified will get added to this list for notification purposes
	 */
	void InsertNewNode(UEdGraphPin* FromPin, UEdGraphPin* NewLinkPin, TSet<UEdGraphNode*>& OutNodeList);

	// UEdGraphNode
	virtual void ReconstructNode() override;
	virtual void AllocateDefaultPins() override;
	// --

	// variants of K2Node methods
	void RewireOldPinsToNewPins(TArray<UEdGraphPin*>& InOldPins);
	void ReconstructSinglePin(UEdGraphPin* NewPin, UEdGraphPin* OldPin);
	// --

	// UEdGraphNode
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	virtual bool CanUserDeleteNode() const override;
	virtual bool CanDuplicateNode() const override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual bool ShowPaletteIconOnNode() const override { return true; }
	virtual FText GetTooltipText() const override;
	virtual void GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const override;
	virtual bool CanJumpToDefinition() const override;
	virtual void JumpToDefinition() const override;
	// --


// Utils
	virtual void GetPopupMessages(TArray<TPair<FString, FLinearColor>>& Messages) const;

	// @todo this data needs to be exposed better. maybe not even made here, but using out data. what about UHeartEdGraphNode make the FEdGraphPinType
	FEdGraphPinType GetEdGraphPinTypeFromPinDesc(const FHeartGraphPinDesc& PinDesc) const;

	void JumpToNodeDefinition() const;


// Pins
public:
	void CreateInputPin(const FHeartGraphPinDesc& PinDesc);
	void CreateOutputPin(const FHeartGraphPinDesc& PinDesc);

	void RemoveOrphanedPin(UEdGraphPin* Pin);

	bool CanUserAddInput() const;
	bool CanUserAddOutput() const;

	bool CanUserRemoveInput(const UEdGraphPin* Pin) const;
	bool CanUserRemoveOutput(const UEdGraphPin* Pin) const;

	void AddUserInput();
	void AddUserOutput();

	// Add pin only on this instance of node, under default pins
	void AddInstancePin(const EEdGraphPinDirection Direction);

	// Call node and graph updates manually, if using bBatchRemoval
	void RemoveInstancePin(UEdGraphPin* Pin);


// Breakpoints
public:
	void OnInputTriggered(const int32 Index);
	void OnOutputTriggered(const int32 Index);

private:
	void TryPausingSession(bool bPauseSession);

	void OnResumePIE(const bool bIsSimulating);
	void OnEndPIE(const bool bIsSimulating);
	void ResetBreakpoints();

public:
	TArray<UEdGraphPin*> InputPins;
	TArray<UEdGraphPin*> OutputPins;

	UPROPERTY()
	FHeartBreakpoint NodeBreakpoint;

	UPROPERTY()
	TMap<FEdGraphPinReference, FHeartBreakpoint> PinBreakpoints;

private:
	UPROPERTY()
	TObjectPtr<UHeartGraphNode> HeartGraphNode;

	bool bBlueprintCompilationPending;
	bool bNeedsFullReconstruction;
};