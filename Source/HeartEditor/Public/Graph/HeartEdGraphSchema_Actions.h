// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraphSchema.h"

#include "Model/HeartGraphNode.h"
#include "HeartEdGraphSchema_Actions.generated.h"

/** Action to add a node to the graph */
USTRUCT()
struct HEARTEDITOR_API FHeartGraphSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	FHeartGraphSchemaAction_NewNode() {}

	FHeartGraphSchemaAction_NewNode(const FHeartNodeArchetype Archetype)
	  : FEdGraphSchemaAction(
			Archetype.GraphNode.GetDefaultObject()->GetDefaultNodeCategory(Archetype.Source),
			Archetype.GraphNode.GetDefaultObject()->GetPreviewNodeTitle(Archetype.Source, EHeartPreviewNodeNameContext::Palette),
			Archetype.GraphNode.GetDefaultObject()->GetDefaultNodeTooltip(Archetype.Source),
			0, // Grouping
			FText::FromString(Archetype.Source.ThisClass()->GetMetaData("Keywords"))),
		Archetype(Archetype) {}

	// FEdGraphSchemaAction
	static FName StaticGetTypeId()
	{
		static const FLazyName Type("FHeartGraphSchemaAction_NewNode");
		return Type;
	}

	virtual FName GetTypeId() const override { return StaticGetTypeId(); }
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override;
	// FEdGraphSchemaAction

	static UHeartEdGraphNode* CreateNode(UEdGraph* ParentGraph, UEdGraphPin* FromPin, FHeartNodeArchetype Archetype, const FVector2D Location, const bool bSelectNewNode = true);

	const UClass* GetNodeClass() const { return Archetype.Source.As<UClass>(); }

private:
	UPROPERTY()
	FHeartNodeArchetype Archetype;
};

/** Action to paste clipboard contents into the graph */
USTRUCT()
struct HEARTEDITOR_API FHeartGraphSchemaAction_Paste : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	FHeartGraphSchemaAction_Paste() {}

	FHeartGraphSchemaAction_Paste(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
	  : FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping) {}

	// FEdGraphSchemaAction
	static FName StaticGetTypeId()
    {
    	static const FLazyName Type("FHeartGraphSchemaAction_Paste");
    	return Type;
    }

    virtual FName GetTypeId() const override { return StaticGetTypeId(); }
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override;
	// FEdGraphSchemaAction
};

/** Action to create new comment */
USTRUCT()
struct HEARTEDITOR_API FHeartGraphSchemaAction_NewComment : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	FHeartGraphSchemaAction_NewComment() {}

	FHeartGraphSchemaAction_NewComment(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
	  : FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping) {}

	// FEdGraphSchemaAction
	static FName StaticGetTypeId()
	{
		static const FLazyName Type("FHeartGraphSchemaAction_NewComment");
		return Type;
	}

	virtual FName GetTypeId() const override { return StaticGetTypeId(); }
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	// FEdGraphSchemaAction
};

/** Action to trigger a runtime linker binding */
USTRUCT()
struct HEARTEDITOR_API FHeartGraphSchemaAction_LinkerBinding : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	FHeartGraphSchemaAction_LinkerBinding() {}

	FHeartGraphSchemaAction_LinkerBinding(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, const FName Key)
	  : FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping), Key(Key) {}

	// FEdGraphSchemaAction
	static FName StaticGetTypeId()
	{
		static const FLazyName Type("FHeartGraphSchemaAction_LinkerBinding");
		return Type;
	}

	virtual FName GetTypeId() const override { return StaticGetTypeId(); }
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override;
	// FEdGraphSchemaAction

	// If this action was summoned by a node, here it be
	UPROPERTY()
	TObjectPtr<UEdGraphNode> ContextNode;

	UPROPERTY()
	FName Key;
};