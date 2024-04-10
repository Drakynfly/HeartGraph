// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "SHeartGraphNodeBase.h"

class SHeartGraphNode_Vertical : public SHeartGraphNodeBase
{
public:
	SLATE_BEGIN_ARGS(SHeartGraphNode_Vertical) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UHeartEdGraphNode* InNode);

	virtual TSharedRef<SWidget> CreateNodeContentArea() override;

	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

	// Variant of SGraphNode::AddPinButtonContent
	virtual void AddPinButton(TSharedPtr<SHorizontalBox> OutputBox, TSharedRef<SWidget> ButtonContent, const EEdGraphPinDirection Direction, FString DocumentationExcerpt = FString(), TSharedPtr<SToolTip> CustomTooltip = nullptr);

protected:
	virtual void UpdateGraphNode() override;

	/** The area where input pins reside */
	TSharedPtr<SHorizontalBox> TopNodeBox;

	/** The area where the node title resides */
	TSharedPtr<SHorizontalBox> CenterNodeBox;

	/** The area where output pins reside */
	TSharedPtr<SHorizontalBox> BottomNodeBox;
};