// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Slate/SHeartGraphWidgetBase.h"

#include "Model/HeartGraph.h"

namespace Heart::Canvas
{
	FLinkerMetadata::FLinkerMetadata(UHeartSlateInputLinker* Linker, const ESlateWidgetType WidgetType)
	  : Linker(Linker),
		WidgetType(WidgetType) {}

	FGraphAndLinkerMetadata::FGraphAndLinkerMetadata(UHeartGraph* Graph, UHeartSlateInputLinker* Linker)
	  : FLinkerMetadata(Linker, ESlateWidgetType::Graph),
		Graph(Graph) {}

	FNodeAndLinkerMetadata::FNodeAndLinkerMetadata(UHeartGraph* Graph, const FHeartNodeGuid& Node, UHeartSlateInputLinker* Linker)
	  : FLinkerMetadata(Linker, ESlateWidgetType::Node),
		Graph(Graph),
		Node(Node) {}

	FPinAndLinkerMetadata::FPinAndLinkerMetadata(UHeartGraph* Graph, const FHeartNodeGuid& Node, const FHeartPinGuid& Pin, UHeartSlateInputLinker* Linker)
	  : FLinkerMetadata(Linker, ESlateWidgetType::Pin),
		Graph(Graph),
		Node(Node),
		Pin(Pin) {}

	HEART_SLATE_INPUT_LINKER_BODY(SGraphWidgetBase)

	void SGraphCanvasWidget::Construct(const FArguments& InArgs)
	{
		AddMetadata(MakeShared<FGraphAndLinkerMetadata>(InArgs._Graph, InArgs._Linker));
	}
	
	void SGraphNodeWidget::Construct(const FArguments& InArgs)
	{
		AddMetadata(MakeShared<FNodeAndLinkerMetadata>(InArgs._Graph, InArgs._NodeGuid, InArgs._Linker));
	}

	void SGraphPinWidget::Construct(const FArguments& InArgs)
	{
		AddMetadata(MakeShared<FPinAndLinkerMetadata>(InArgs._Graph, InArgs._NodeGuid, InArgs._PinGuid, InArgs._Linker));
	}
}