// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartSlateInputLinker.h"
#include "Widgets/SCompoundWidget.h"

class UHeartGraph;
class UHeartGraphNode;

namespace Heart::Canvas
{
	enum ESlateWidgetType
	{
		None,
		Graph,
		Node,
		Pin,

		// note: doesn't do anything, but here for later
		Connection
	};

	/**
	 * RTTI for heart's input and visualization reflection
	 */
	class HEARTCANVAS_API FLinkerMetadata : public ISlateMetaData
	{
	public:
		SLATE_METADATA_TYPE(FNodeAndLinkerMetadata, ISlateMetaData)

		FLinkerMetadata(UHeartSlateInputLinker* Linker, ESlateWidgetType WidgetType);

		TWeakObjectPtr<UHeartSlateInputLinker> Linker;

		ESlateWidgetType WidgetType;
	};

	/**
	 * RTTI for heart's input and visualization reflection
	 */
	class HEARTCANVAS_API FGraphAndLinkerMetadata : public FLinkerMetadata
	{
	public:
		SLATE_METADATA_TYPE(FGraphAndLinkerMetadata, FLinkerMetadata)

		FGraphAndLinkerMetadata(UHeartGraph* Graph, UHeartSlateInputLinker* Linker);

		TWeakObjectPtr<UHeartGraph> Graph;
	};

	/**
	 * RTTI for heart's input and visualization reflection
	 */
	class HEARTCANVAS_API FNodeAndLinkerMetadata : public FLinkerMetadata
	{
	public:
		SLATE_METADATA_TYPE(FNodeAndLinkerMetadata, FLinkerMetadata)

		FNodeAndLinkerMetadata(UHeartGraphNode* Node, UHeartSlateInputLinker* Linker, ESlateWidgetType WidgetType);

		TWeakObjectPtr<UHeartGraphNode> Node;
	};

	/**
	 * An example base class for a Node or Pin slate widget
	 */
	class HEARTCANVAS_API SGraphWidgetBase : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SGraphWidgetBase)
		{}
			SLATE_ARGUMENT( UHeartGraphNode*, GraphNode )
			SLATE_ARGUMENT( UHeartSlateInputLinker*, Linker )
			SLATE_ARGUMENT( ESlateWidgetType, Type )
		SLATE_END_ARGS()

		HEART_SLATE_INPUT_LINKER_HEADER(SCompoundWidget);

		/** Constructs this widget with InArgs */
		void Construct(const FArguments& InArgs);
	};
}