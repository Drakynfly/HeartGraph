// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Slate/SHeartGraphWidgetBase.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

namespace Heart::Canvas
{
	HEART_SLATE_INPUT_LINKER_BODY(SGraphWidgetBase)

	FLinkerMetadata::FLinkerMetadata(UHeartSlateInputLinker* Linker, const ESlateWidgetType WidgetType)
	  : Linker(Linker),
		WidgetType(WidgetType) {}

	FGraphAndLinkerMetadata::FGraphAndLinkerMetadata(UHeartGraph* Graph, UHeartSlateInputLinker* Linker)
	  : FLinkerMetadata(Linker, ESlateWidgetType::Graph),
		Graph(Graph) {}

	FNodeAndLinkerMetadata::FNodeAndLinkerMetadata(UHeartGraphNode* Node, UHeartSlateInputLinker* Linker, const ESlateWidgetType WidgetType)
	  : FLinkerMetadata(Linker, WidgetType),
		Node(Node) {}

	void SGraphWidgetBase::Construct(const FArguments& InArgs)
	{
		AddMetadata(MakeShared<FNodeAndLinkerMetadata>(InArgs._GraphNode, InArgs._Linker, InArgs._Type));
	}
}