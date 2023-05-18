// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "SHeartGraphNodeBase.h"

class SHeartGraphNode_Horizontal : public SHeartGraphNodeBase
{
public:
	SLATE_BEGIN_ARGS(SHeartGraphNode_Horizontal) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UHeartEdGraphNode* InNode);

	virtual TSharedRef<SWidget> CreateNodeContentArea() override;

	// Variant of SGraphNode::AddPinButtonContent
	virtual void AddPinButton(TSharedPtr<SVerticalBox> OutputBox, TSharedRef<SWidget> ButtonContent, const EEdGraphPinDirection Direction, FString DocumentationExcerpt = FString(), TSharedPtr<SToolTip> CustomTooltip = nullptr);

protected:
	virtual void UpdateGraphNode() override;
};
