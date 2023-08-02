// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraph.h"
#include "Model/HeartGraphTypes.h"

#include "HeartEdGraph.generated.h"

class UHeartEdGraphNode;
class UHeartGraph;
class UHeartGraphNode;

UCLASS()
class HEARTEDITOR_API UHeartEdGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	UHeartEdGraph(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitProperties() override;
	virtual void PostLoad() override;

	static UEdGraph* CreateGraph(UHeartGraph* InHeartGraph);

	UHeartEdGraphNode* FindEdGraphNode(const TFunction<bool(const UHeartEdGraphNode*)>& Iter);
	UHeartEdGraphNode* FindEdGraphNodeForNode(const UHeartGraphNode* HeartGraphNode);

	/** Returns the HeartGraph that contains this UEdGraph */
	UHeartGraph* GetHeartGraph() const;

private:
	void OnNodeCreatedInEditorExternally(UHeartGraphNode* Node);
	void OnNodeAdded(UHeartGraphNode* HeartGraphNode);
	void OnNodeRemoved(UHeartGraphNode* HeartGraphNode);
	void OnNodeConnectionsChanged(const FHeartGraphConnectionEvent& HeartGraphConnectionEvent);
};