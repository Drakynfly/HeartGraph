// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "Templates/SubclassOf.h"

#include "HeartEdGraphNode.generated.h"

class UEdGraphSchema;
class UHeartGraphNode;
class UHeartGraphPin;

USTRUCT()
struct HEARTEDITOR_API FHeartBreakpoint
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHasBreakpoint;

	bool bBreakpointEnabled;
	bool bBreakpointHit;

	FHeartBreakpoint()
	{
		bHasBreakpoint = false;
		bBreakpointEnabled = false;
		bBreakpointHit = false;
	};

	void AddBreakpoint();
	void RemoveBreakpoint();
	bool HasBreakpoint() const;

	void EnableBreakpoint();
	bool CanEnableBreakpoint() const;

	void DisableBreakpoint();
	bool IsBreakpointEnabled() const;

	void ToggleBreakpoint();
};

/**
 * Graph representation of a Heart Node in an EdGraph
 */
UCLASS()
class HEARTEDITOR_API UHeartEdGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UHeartEdGraphNode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

//////////////////////////////////////////////////////////////////////////
// Heart Graph Node

private:
	UPROPERTY(Instanced)
	UHeartGraphNode* HeartGraphNode;

	bool bBlueprintCompilationPending;
	bool bNeedsFullReconstruction;

public:
	// It would be intuitive to assign a custom Graph Node class in Heart Graph Node class
	// However, we shouldn't assign class from editor module to runtime module class
	UPROPERTY()
	TArray<TSubclassOf<UHeartGraphNode>> AssignedNodeClasses;

	void SetHeartGraphNode(UHeartGraphNode* InHeartGraphNode);
	UHeartGraphNode* GetHeartGraphNode() const;

	// UObject
	virtual void PostLoad() override;
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual void PostEditImport() override;
	// --

	// UEdGraphNode
	virtual void PostPlacedNewNode() override;
	virtual void PrepareForCopying() override;
    // --

	void PostCopyNode();

private:
	void SubscribeToExternalChanges();

	void OnBlueprintPreCompile(UBlueprint* Blueprint);
	void OnBlueprintCompiled();

	UFUNCTION()
	void OnExternalChange(UHeartGraphNode* Node);

//////////////////////////////////////////////////////////////////////////
// Graph node

public:
	UPROPERTY()
	FHeartBreakpoint NodeBreakpoint;

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
	// --

//////////////////////////////////////////////////////////////////////////
// Utils

	UHeartGraphPin* GetPinByName(const FName& Name) const;

public:
	// UEdGraphNode
	virtual bool CanJumpToDefinition() const override;
	virtual void JumpToDefinition() const override;
	// --

//////////////////////////////////////////////////////////////////////////
// Pins

public:
	TArray<UEdGraphPin*> InputPins;
	TArray<UEdGraphPin*> OutputPins;

	UPROPERTY()
	TMap<FEdGraphPinReference, FHeartBreakpoint> PinBreakpoints;

	void CreateInputPin(const UHeartGraphPin* HeartPin, const int32 Index = INDEX_NONE);
	void CreateOutputPin(const UHeartGraphPin* HeartPin, const int32 Index = INDEX_NONE);

	void RemoveOrphanedPin(UEdGraphPin* Pin);

	bool SupportsDynamicPins() const;

	bool CanUserAddInput() const;
	bool CanUserAddOutput() const;

	bool CanUserRemoveInput(const UEdGraphPin* Pin) const;
	bool CanUserRemoveOutput(const UEdGraphPin* Pin) const;

	void AddUserInput();
	void AddUserOutput();

	// Add pin only on this instance of node, under default pins
	void AddInstancePin(const EEdGraphPinDirection Direction, const uint8 NumberedPinsAmount);

	// Call node and graph updates manually, if using bBatchRemoval
	void RemoveInstancePin(UEdGraphPin* Pin);

	// Create pins from the context asset, i.e. Sequencer events
	void RefreshDynamicPins(const bool bReconstructNode);

	// UEdGraphNode
	virtual void GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const override;
	// --

//////////////////////////////////////////////////////////////////////////
// Breakpoints

public:
	void OnInputTriggered(const int32 Index);
	void OnOutputTriggered(const int32 Index);

private:
	void TryPausingSession(bool bPauseSession);

	void OnResumePIE(const bool bIsSimulating);
	void OnEndPIE(const bool bIsSimulating);
	void ResetBreakpoints();
};
