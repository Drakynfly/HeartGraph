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

	FHeartGraphSchemaAction_NewNode(const FHeartNodeSource NodeSource, const UHeartGraphNode* GraphNode)
	  : FEdGraphSchemaAction(
			GraphNode->GetDefaultNodeCategory(NodeSource),
			GraphNode->GetPreviewNodeTitle(NodeSource, EHeartPreviewNodeNameContext::Palette),
			GraphNode->GetDefaultNodeTooltip(NodeSource),
			0, // Grouping
			FText::FromString(NodeSource.ThisClass()->GetMetaData("Keywords"))),
		NodeSource({NodeSource, GraphNode->GetClass()}) {}

	FHeartGraphSchemaAction_NewNode(const FHeartNodeSourceAndGraphNode NodeSourceAndGraphNode)
	  : FEdGraphSchemaAction(
			NodeSourceAndGraphNode.GraphNode.GetDefaultObject()->GetDefaultNodeCategory(NodeSourceAndGraphNode.Source),
			NodeSourceAndGraphNode.GraphNode.GetDefaultObject()->GetPreviewNodeTitle(NodeSourceAndGraphNode.Source, EHeartPreviewNodeNameContext::Palette),
			NodeSourceAndGraphNode.GraphNode.GetDefaultObject()->GetDefaultNodeTooltip(NodeSourceAndGraphNode.Source),
			0, // Grouping
			FText::FromString(NodeSourceAndGraphNode.Source.ThisClass()->GetMetaData("Keywords"))),
		NodeSource(NodeSourceAndGraphNode) {}

	// FEdGraphSchemaAction
	static FName StaticGetTypeId()
	{
		static FName Type("FHeartGraphSchemaAction_NewNode");
		return Type;
	}

	virtual FName GetTypeId() const override { return StaticGetTypeId(); }
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	// FEdGraphSchemaAction

	static UHeartEdGraphNode* CreateNode(UEdGraph* ParentGraph, UEdGraphPin* FromPin, FHeartNodeSourceAndGraphNode NodeSource, const FVector2D Location, const bool bSelectNewNode = true);

	const UClass* GetNodeClass() const { return NodeSource.Source.As<UClass>(); }

private:
	UPROPERTY()
	FHeartNodeSourceAndGraphNode NodeSource;
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
    	static FName Type("FHeartGraphSchemaAction_Paste");
    	return Type;
    }

    virtual FName GetTypeId() const override { return StaticGetTypeId(); }
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
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
		static FName Type("FHeartGraphSchemaAction_NewComment");
		return Type;
	}

	virtual FName GetTypeId() const override { return StaticGetTypeId(); }
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	// FEdGraphSchemaAction
};