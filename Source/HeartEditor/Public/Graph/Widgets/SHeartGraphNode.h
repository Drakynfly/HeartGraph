// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "SGraphNode.h"
#include "SGraphPin.h"
#include "Nodes/HeartEdGraphNode.h"

class HEARTEDITOR_API SHeartGraphPin : public SGraphPin
{
public:
	SHeartGraphPin();

	SLATE_BEGIN_ARGS(SHeartGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
};

class HEARTEDITOR_API SHeartGraphNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SHeartGraphNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UHeartEdGraphNode* InNode);

protected:
	// SNodePanel::SNode
	virtual void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override;
	virtual const FSlateBrush* GetShadowBrush(bool bSelected) const override;
	virtual void GetOverlayBrushes(bool bSelected, const FVector2D WidgetSize, TArray<FOverlayBrushInfo>& Brushes) const override;
	// --

	virtual void GetPinBrush(const bool bLeftSide, const float WidgetWidth, const int32 PinIndex, const FHeartBreakpoint& Breakpoint, TArray<FOverlayBrushInfo>& Brushes) const;

	// SGraphNode
	virtual void UpdateGraphNode() override;
	virtual void UpdateErrorInfo() override;

	virtual TSharedRef<SWidget> CreateTitleWidget(TSharedPtr<SNodeTitle> NodeTitle) override;
	virtual TSharedRef<SWidget> CreateNodeContentArea() override;
	virtual const FSlateBrush* GetNodeBodyBrush() const override;

	// purposely overriden non-virtual methods, avoiding engine modification
	FSlateColor GetNodeTitleColor() const;
	FSlateColor GetNodeBodyColor() const;
	FSlateColor GetNodeTitleIconColor() const;
	FLinearColor GetNodeTitleTextColor() const;

	virtual void CreateStandardPinWidget(UEdGraphPin* Pin) override;
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;

	virtual void CreateInputSideAddButton(TSharedPtr<SVerticalBox> OutputBox) override;
	virtual void CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox) override;
	// --

	// Variant of SGraphNode::AddPinButtonContent
	virtual void AddPinButton(TSharedPtr<SVerticalBox> OutputBox, TSharedRef<SWidget> ButtonContent, const EEdGraphPinDirection Direction, FString DocumentationExcerpt = FString(), TSharedPtr<SToolTip> CustomTooltip = nullptr);

	// Variant of SGraphNode::OnAddPin
	virtual FReply OnAddHeartPin(const EEdGraphPinDirection Direction);

protected:
	UHeartEdGraphNode* HeartEdGraphNode = nullptr;
};
