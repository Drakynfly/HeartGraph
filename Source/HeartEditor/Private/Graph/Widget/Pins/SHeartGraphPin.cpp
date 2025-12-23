// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/Widgets/Pins/SHeartGraphPin.h"
#include "Model/HeartGraph.h"
#include "Nodes/HeartEdGraphNode.h"
#include "Slate/SHeartGraphWidgetBase.h"

SHeartGraphPin::SHeartGraphPin()
{
}

void SHeartGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InPin);

	const UHeartEdGraphNode* EdGraphNode = Cast<UHeartEdGraphNode>(GraphPinObj->GetOwningNode());

	UHeartGraph* Graph = EdGraphNode->GetHeartGraph();
	FHeartNodeGuid NodeGuid = EdGraphNode->GetNodeGuid();
	const FHeartPinGuid PinGuid = Graph->FindNodePin(NodeGuid, InPin->PinName);

	if (PinGuid.IsValid())
	{
		AddMetadata(MakeShared<Heart::Canvas::FPinAndLinkerMetadata>(Graph, NodeGuid, PinGuid, InArgs._Linker));
	}

	// @todo expose setting this from the schema
	//bUsePinColorForText = true;
}