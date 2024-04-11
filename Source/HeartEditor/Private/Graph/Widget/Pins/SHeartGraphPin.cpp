// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/Widgets/Pins/SHeartGraphPin.h"
#include "Model/HeartGraphNode.h"
#include "Nodes/HeartEdGraphNode.h"
#include "Slate/SHeartGraphWidgetBase.h"

SHeartGraphPin::SHeartGraphPin()
{
}

void SHeartGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InPin);

	UHeartGraphNode* GraphNode = Cast<UHeartEdGraphNode>(GraphPinObj->GetOwningNode())->GetHeartGraphNode();
	FHeartPinGuid PinGuid;

	if (IsValid(GraphNode))
	{
		if (auto Option = GraphNode->QueryPins().Find(
				[&InPin](const FHeartGraphPinDesc& Desc)
				{
					return Desc.Name == InPin->PinName;
				});
			Option.IsSet())
		{
			PinGuid = Option.GetValue();
		}
	}

	AddMetadata(MakeShared<Heart::Canvas::FPinAndLinkerMetadata>(GraphNode, PinGuid, InArgs._Linker));

	// @todo expose setting this from the schema
	//bUsePinColorForText = true;
}