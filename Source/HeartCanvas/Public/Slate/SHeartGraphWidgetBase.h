// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Move_To_UMG/HeartSlateInputLinker.h"
#include "Widgets/SCompoundWidget.h"

/**
 *
 */
class HEARTCANVAS_API SHeartGraphWidgetBase : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHeartGraphWidgetBase) {}
	SLATE_END_ARGS()

	HEART_SLATE_INPUT_LINKER_HEADER(SCompoundWidget);

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};