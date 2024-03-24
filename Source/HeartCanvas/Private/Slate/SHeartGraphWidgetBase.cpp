// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Slate/SHeartGraphWidgetBase.h"

#include "SlateOptMacros.h"

FReply SHeartGraphWidgetBase::OnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	auto&& Reply = Heart::Input::LinkOnMouseWheel<SWidget>(SharedThis(this), InGeometry, InMouseEvent);
	return Reply.IsEventHandled() ? Reply : Super::OnMouseWheel(InGeometry, InMouseEvent);
}

FReply SHeartGraphWidgetBase::OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	auto&& Reply = Heart::Input::LinkOnMouseButtonDown<SWidget>(SharedThis(this), InGeometry, InMouseEvent);
	return Reply.IsEventHandled() ? Reply : Super::OnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply SHeartGraphWidgetBase::OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	auto&& Reply = Heart::Input::LinkOnMouseButtonUp<SWidget>(SharedThis(this), InGeometry, InMouseEvent);
	return Reply.IsEventHandled() ? Reply : Super::OnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply SHeartGraphWidgetBase::OnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	auto&& Reply = Heart::Input::LinkOnKeyDown<SWidget>(SharedThis(this), InGeometry, InKeyEvent);
	return Reply.IsEventHandled() ? Reply : Super::OnKeyDown(InGeometry, InKeyEvent);
}

FReply SHeartGraphWidgetBase::OnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	auto&& Reply = Heart::Input::LinkOnKeyUp<SWidget>(SharedThis(this), InGeometry, InKeyEvent);
	return Reply.IsEventHandled() ? Reply : Super::OnKeyUp(InGeometry, InKeyEvent);
}

FReply SHeartGraphWidgetBase::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return Super::OnDragDetected(MyGeometry, MouseEvent);

	// @todo
	/*
	if (!Heart::Input::LinkOnDragDetected<SWidget>(SharedThis(this), MyGeometry, MouseEvent, OutOperation))
	{

	}
	*/
}

FReply SHeartGraphWidgetBase::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	return Super::OnDrop(MyGeometry, DragDropEvent);

	// @todo
	/*
	bool Handled = Heart::Input::LinkOnDrop<SWidget>(SharedThis(this), MyGeometry, DragDropEvent, InOperation);
	return Handled ? true : Super::OnDrop(MyGeometry, DragDropEvent);
	*/
}

FReply SHeartGraphWidgetBase::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	return Super::OnDragOver(MyGeometry, DragDropEvent);

	// @todo
	/*
	bool Handled = Heart::Input::LinkOnDragOver<SWidget>(SharedThis(this), MyGeometry, DragDropEvent, InOperation);
	return Handled ? true : Super::OnDragOver(MyGeometry, DragDropEvent);
	*/
}

void SHeartGraphWidgetBase::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	Super::OnDragEnter(MyGeometry, DragDropEvent);

	// @todo
	/*
	Heart::Input::LinkOnDragEnter<SWidget>(SharedThis(this), MyGeometry, DragDropEvent);
	*/
}

void SHeartGraphWidgetBase::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
	Super::OnDragLeave(DragDropEvent);

	// @todo
	/*
	Heart::Input::LinkOnDragLeave<SWidget>(SharedThis(this), InDragDropEvent);
	*/
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SHeartGraphWidgetBase::Construct(const FArguments& InArgs)
{
	/*
	ChildSlot
	[
		// Populate the widget
	];
	*/
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION