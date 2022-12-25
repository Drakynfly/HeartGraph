// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraph.h"

#include "HeartEdGraph.generated.h"

class UHeartGraph;
class UHeartGraphNode;

UCLASS()
class HEARTEDITOR_API UHeartEdGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	UHeartEdGraph(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostLoad() override;

	static UEdGraph* CreateGraph(UHeartGraph* InHeartGraph);

	/** Returns the HeartGraph that contains this UEdGraph */
	UHeartGraph* GetHeartGraph() const;

private:
	void OnNodeCreatedInEditorExternally(UHeartGraphNode* Node);
};
