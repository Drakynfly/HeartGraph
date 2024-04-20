// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Slate/SHeartGraphWidgetBase.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

namespace Heart::Canvas
{
	FLinkerMetadata::FLinkerMetadata(UHeartSlateInputLinker* Linker, const ESlateWidgetType WidgetType)
	  : Linker(Linker),
		WidgetType(WidgetType) {}

	FGraphAndLinkerMetadata::FGraphAndLinkerMetadata(UHeartGraph* Graph, UHeartSlateInputLinker* Linker)
	  : FLinkerMetadata(Linker, ESlateWidgetType::Graph),
		Graph(Graph) {}

	FNodeAndLinkerMetadata::FNodeAndLinkerMetadata(UHeartGraphNode* Node, UHeartSlateInputLinker* Linker)
	  : FLinkerMetadata(Linker, ESlateWidgetType::Node),
		Node(Node) {}

	FPinAndLinkerMetadata::FPinAndLinkerMetadata(UHeartGraphNode* Node, const FHeartPinGuid Pin, UHeartSlateInputLinker* Linker)
	  : FLinkerMetadata(Linker, ESlateWidgetType::Pin),
		Node(Node),
		Pin(Pin) {}

	HEART_SLATE_INPUT_LINKER_BODY(SGraphWidgetBase)

	void SGraphCanvasWidget::Construct(const FArguments& InArgs)
	{
		AddMetadata(MakeShared<FGraphAndLinkerMetadata>(InArgs._Graph, InArgs._Linker));
	}
	
	void SGraphNodeWidget::Construct(const FArguments& InArgs)
	{
		AddMetadata(MakeShared<FNodeAndLinkerMetadata>(InArgs._GraphNode, InArgs._Linker));
	}

	void SGraphPinWidget::Construct(const FArguments& InArgs)
	{
		AddMetadata(MakeShared<FPinAndLinkerMetadata>(InArgs._GraphNode, InArgs._PinGuid, InArgs._Linker));
	}
}