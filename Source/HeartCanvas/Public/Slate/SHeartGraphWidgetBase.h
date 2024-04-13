// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartSlateInputLinker.h"
#include "Model/HeartGuids.h"
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
		SLATE_METADATA_TYPE(FLinkerMetadata, ISlateMetaData)

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

		FNodeAndLinkerMetadata(UHeartGraphNode* Node, UHeartSlateInputLinker* Linker);

		TWeakObjectPtr<UHeartGraphNode> Node;
	};

	/**
	 * RTTI for heart's input and visualization reflection
	 */
	class HEARTCANVAS_API FPinAndLinkerMetadata : public FLinkerMetadata
	{
	public:
		SLATE_METADATA_TYPE(FPinAndLinkerMetadata, FLinkerMetadata)

		FPinAndLinkerMetadata(UHeartGraphNode* Node, FHeartPinGuid Pin, UHeartSlateInputLinker* Linker);

		TWeakObjectPtr<UHeartGraphNode> Node;
		FHeartPinGuid Pin;
	};

	/**
	 * A base class for a heart slate widget. All this does is setup input linking
	 */
	class HEARTCANVAS_API SGraphWidgetBase : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SGraphWidgetBase) {}
		SLATE_END_ARGS()

		HEART_SLATE_INPUT_LINKER_HEADER(SCompoundWidget);
	};

	/**
	 * A base class for a HeartGraph canvas slate widget
	 */
	class HEARTCANVAS_API SGraphCanvasWidget : public SGraphWidgetBase
	{
	public:
		SLATE_BEGIN_ARGS(SGraphCanvasWidget)
		{}
			SLATE_ARGUMENT(UHeartGraph*, Graph)
			SLATE_ARGUMENT(UHeartSlateInputLinker*, Linker)
		SLATE_END_ARGS()

		/** Constructs this widget with InArgs */
		void Construct(const FArguments& InArgs);
	};

	/**
	 * A base class for a HeartNode slate widget
	 */
	class HEARTCANVAS_API SGraphNodeWidget : public SGraphWidgetBase
	{
	public:
		SLATE_BEGIN_ARGS(SGraphNodeWidget)
		{}
			SLATE_ARGUMENT( UHeartGraphNode*, GraphNode )
			SLATE_ARGUMENT( UHeartSlateInputLinker*, Linker )
		SLATE_END_ARGS()

		/** Constructs this widget with InArgs */
		void Construct(const FArguments& InArgs);
	};

	/**
	 * A base class for a HeartPin slate widget
	 */
	class HEARTCANVAS_API SGraphPinWidget : public SGraphWidgetBase
	{
	public:
		SLATE_BEGIN_ARGS(SGraphPinWidget)
		{}
			SLATE_ARGUMENT( UHeartGraphNode*, GraphNode )
			SLATE_ARGUMENT( FHeartPinGuid, PinGuid)
			SLATE_ARGUMENT( UHeartSlateInputLinker*, Linker )
		SLATE_END_ARGS()

		/** Constructs this widget with InArgs */
		void Construct(const FArguments& InArgs);
	};
}