// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartSlateInputLinker.h"
#include "Widgets/SCompoundWidget.h"

class UHeartGraphNode;

namespace Heart::Canvas
{
	enum ESlateWidgetType
	{
		None,
		Pin,
		Node,

		// note: doesnt do anything, but here for later
		Connection
	};

	/**
	 * RTTI for heart's input and visualization reflection
	 */
	class HEARTCANVAS_API FNodeAndLinkerMetadata : public ISlateMetaData
	{
	public:
		SLATE_METADATA_TYPE(FNodeAndLinkerMetadata, ISlateMetaData)

		FNodeAndLinkerMetadata(UHeartGraphNode* Node, UHeartSlateInputLinker* Linker, ESlateWidgetType WidgetType);

		TWeakObjectPtr<UHeartGraphNode> Node;

		TWeakObjectPtr<UHeartSlateInputLinker> Linker;

		ESlateWidgetType WidgetType;
	};

	/**
	 *
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