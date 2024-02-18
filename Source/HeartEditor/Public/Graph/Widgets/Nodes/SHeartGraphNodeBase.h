// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "SGraphNode.h"
#include "Nodes/HeartEdGraphNode.h"

// Base class for SGraphNodes in heart graphs
class HEARTEDITOR_API SHeartGraphNodeBase : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SHeartGraphNodeBase) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UHeartEdGraphNode* InNode);

	template <typename TGraphNodeClass>
	static TSharedRef<SGraphNode> MakeInstance(UHeartEdGraphNode* InNode)
	{
		static_assert(TIsDerivedFrom<TGraphNodeClass, SHeartGraphNodeBase>::Value, TEXT("TGraphNodeClass should derive from SHeartGraphNodeBase..."));
		static_assert(!std::is_same_v<TGraphNodeClass, SHeartGraphNodeBase>, TEXT("... but not be SHeartGraphNodeBase itself!"));
		return SNew(TGraphNodeClass, InNode);
	}

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

	virtual void CreateStandardPinWidget(UEdGraphPin* Pin) override;
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;
	// --

	// Variant of SGraphNode::CreateInputSideAddButton
	virtual TSharedPtr<SWidget> CreateAddInputButton();

	// Variant of SGraphNode::CreateInputSideAddButton
	virtual TSharedPtr<SWidget> CreateAddOutputButton();

	// Variant of SGraphNode::OnAddPin
	virtual FReply OnAddHeartPin(const EEdGraphPinDirection Direction);

	void CreateCommentBubble();

protected:
	UHeartEdGraphNode* HeartEdGraphNode = nullptr;
};