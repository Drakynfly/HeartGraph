// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraph.h"
#include "Model/HeartGraphInterface.h"
#include "Model/HeartGraphTypes.h"
#include "ModelView/Actions/HeartGraphAction.h"

#include "HeartEdGraph.generated.h"

class UHeartSlateInputLinker;
class UHeartEdGraphNode;
class UHeartGraph;
class UHeartGraphNode;

/**
 * A test action to debug running graph actions via the editor.
 */
UCLASS()
class UHeartEditorDebugAction : public UHeartGraphAction
{
	GENERATED_BODY()

public:
	virtual FText GetDescription(const UObject* Target) const override;
	virtual bool CanExecute(const UObject* Target) const override;
	virtual FHeartEvent ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
	virtual FHeartEvent ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
	virtual FHeartEvent ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
};


UCLASS()
class HEARTEDITOR_API UHeartEdGraph : public UEdGraph, public IHeartGraphInterface
{
	GENERATED_BODY()

public:
	UHeartEdGraph(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitProperties() override;
	virtual void PostLoad() override;

	static UEdGraph* CreateGraph(UHeartGraph* InHeartGraph);

	UHeartEdGraphNode* FindEdGraphNode(const TFunction<bool(const UHeartEdGraphNode*)>& Iter);
	UHeartEdGraphNode* FindEdGraphNodeForNode(const UHeartGraphNode* HeartGraphNode);

	// IHeartGraphInterface
	virtual UHeartGraph* GetHeartGraph() const override;
	// IHeartGraphInterface


	UHeartSlateInputLinker* GetEditorLinker() const;

protected:
	void CreateSlateInputLinker();

private:
	// Create the EdGraph node equivilent to a HeartNode
	void CreateEdGraphNode(UHeartGraphNode* Node);

	void OnNodeAdded(UHeartGraphNode* HeartGraphNode);
	void OnNodeRemoved(UHeartGraphNode* HeartGraphNode);
	void OnNodeConnectionsChanged(const FHeartGraphConnectionEvent& HeartGraphConnectionEvent);

	// Transient, so it can be remade on PostLoad/CreateGraph since the class used, change be changed by the schema.
	UPROPERTY(Transient)
	TObjectPtr<UHeartSlateInputLinker> SlateInputLinker;
};