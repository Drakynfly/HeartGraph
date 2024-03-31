// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/Widgets/Pins/SHeartGraphPin.h"
#include "Nodes/HeartEdGraphNode.h"
#include "Slate/SHeartGraphWidgetBase.h"

SHeartGraphPin::SHeartGraphPin()
{
}

void SHeartGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InPin);

	auto GraphNode = Cast<UHeartEdGraphNode>(GraphPinObj->GetOwningNode())->GetHeartGraphNode();

	AddMetadata(MakeShared<Heart::Canvas::FNodeAndLinkerMetadata>(
		GraphNode, InArgs._Linker, Heart::Canvas::Node));

	// @todo expose setting this from the schema
	//bUsePinColorForText = true;
}