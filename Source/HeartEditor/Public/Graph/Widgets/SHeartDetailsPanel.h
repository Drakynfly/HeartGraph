// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Widgets/SCompoundWidget.h"

class IDetailsView;

namespace Heart::AssetEditor
{
	class FHeartGraphEditor;

	/**
	 * A split details panel showing two detail views, one for the Graph / GraphNode and one for the NodeObject
	 */
	class HEARTEDITOR_API SDetailsPanel : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SDetailsPanel)
		{
		}
		SLATE_END_ARGS()

		/** Constructs this widget with InArgs */
		void Construct(const FArguments& InArgs, const TSharedPtr<FHeartGraphEditor>& AssetEditor);

		TSharedPtr<IDetailsView> DetailsView_Graph;
		TSharedPtr<IDetailsView> DetailsView_Object;
	};
}


