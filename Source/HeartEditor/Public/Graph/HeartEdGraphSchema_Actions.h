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

	UPROPERTY()
	UClass* NodeClass;

	static FName StaticGetTypeId()
	{
		static FName Type("FHeartGraphSchemaAction_NewNode");
		return Type;
	}

	virtual FName GetTypeId() const override { return StaticGetTypeId(); }

	FHeartGraphSchemaAction_NewNode()
		: FEdGraphSchemaAction()
		, NodeClass(nullptr)
	{
	}

	FHeartGraphSchemaAction_NewNode(UClass* Node)
		: FEdGraphSchemaAction()
		, NodeClass(Node)
	{
	}

	FHeartGraphSchemaAction_NewNode(const UObject* Node, const UHeartGraphNode* GraphNode)
		: FEdGraphSchemaAction(
			GraphNode->GetDefaultNodeCategory(Node),
			GraphNode->GetDefaultNodeTitle(Node),
			GraphNode->GetDefaultNodeToolTip(Node),
			0, FText::FromString(Node->GetClass()->GetMetaData("Keywords")))
		, NodeClass(Node->GetClass())
	{
	}

	// FEdGraphSchemaAction
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	// --

	static UHeartEdGraphNode* CreateNode(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, UClass* NodeClass, const FVector2D Location, const bool bSelectNewNode = true);
};

/** Action to paste clipboard contents into the graph */
USTRUCT()
struct HEARTEDITOR_API FHeartGraphSchemaAction_Paste : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	FHeartGraphSchemaAction_Paste()
		: FEdGraphSchemaAction()
	{
	}

	FHeartGraphSchemaAction_Paste(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
	{
	}

	// FEdGraphSchemaAction
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	// --
};

/** Action to create new comment */
USTRUCT()
struct HEARTEDITOR_API FHeartGraphSchemaAction_NewComment : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	// Simple type info
	static FName StaticGetTypeId()
	{
		static FName Type("FHeartGraphSchemaAction_NewComment");
		return Type;
	}

	virtual FName GetTypeId() const override { return StaticGetTypeId(); }

	FHeartGraphSchemaAction_NewComment()
		: FEdGraphSchemaAction()
	{
	}

	FHeartGraphSchemaAction_NewComment(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
	{
	}

	// FEdGraphSchemaAction
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	// --
};
