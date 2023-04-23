// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraphSchema.h"

#include "Model/HeartGraphNode.h"
#include "Nodes/HeartEdGraphNode.h"
#include "HeartEdGraphSchema_Actions.generated.h"

/** Action to add a node to the graph */
USTRUCT()
struct HEARTEDITOR_API FHeartGraphSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	FHeartGraphSchemaAction_NewNode() {}

	FHeartGraphSchemaAction_NewNode(UClass* Node)
	  :  NodeClass(Node) {}

	FHeartGraphSchemaAction_NewNode(const UClass* NodeClass, const UHeartGraphNode* GraphNode)
	  : FEdGraphSchemaAction(
			GraphNode->GetDefaultNodeCategory(NodeClass),
			GraphNode->GetDefaultNodeTitle<EHeartNodeNameContext::Palette>(NodeClass),
			GraphNode->GetDefaultNodeTooltip(NodeClass),
			0, FText::FromString(NodeClass->GetMetaData("Keywords"))),
		NodeClass(NodeClass) {}

	// FEdGraphSchemaAction
	static FName StaticGetTypeId()
	{
		static FName Type("FHeartGraphSchemaAction_NewNode");
		return Type;
	}

	virtual FName GetTypeId() const override { return StaticGetTypeId(); }
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	// FEdGraphSchemaAction

	static UHeartEdGraphNode* CreateNode(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const UClass* NodeClass, const FVector2D Location, const bool bSelectNewNode = true);

	const UClass* GetNodeClass() const { return NodeClass; }

private:
	UPROPERTY()
	TObjectPtr<const UClass> NodeClass = nullptr;
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
