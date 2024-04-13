// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartInputLinkerBase.h"
#include "HeartSlateInputLinker.generated.h"

struct FHeartManualEvent;

UCLASS()
class HEARTCANVAS_API UHeartSlateInputLinker : public UHeartInputLinkerBase
{
	GENERATED_BODY()

public:
	// Regular mouse / keyboard / game-pad events
	virtual FReply HandleOnMouseWheel(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	virtual FReply HandleOnMouseButtonDown(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	virtual FReply HandleOnMouseButtonUp(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent);
	virtual FReply HandleOnKeyDown(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent);
	virtual FReply HandleOnKeyUp(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent);

	// Drag drop events
	virtual FReply HandleOnDragDetected(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	virtual FReply HandleOnDrop(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
	virtual FReply HandleOnDragOver(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
	virtual void HandleOnDragEnter(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
	virtual void HandleOnDragLeave(const TSharedRef<SWidget>& Widget, const FDragDropEvent& DragDropEvent);
};

template <>
struct THeartInputLinkerType<SWidget>
{
	static constexpr bool Supported = true;

	using FReplyType = FReply;
	using FValueType = const TSharedRef<SWidget>&;
	using FDDOType = FReply;

	static FReplyType NoReply() { return FReply::Unhandled(); }

	HEARTCANVAS_API static UHeartSlateInputLinker* FindLinker(const TSharedRef<SWidget>& Widget);
};

/**
 * Place this macro after the SLATE_END_ARGS or Construct of a SWidget derived class you wish to implement an InputLinker on.
 */
#define HEART_SLATE_INPUT_LINKER_HEADER(SuperType)																	   \
using Super = SuperType;                                                                                               \
protected:                                                                                                             \
virtual FReply OnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;                  \
virtual FReply OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;             \
virtual FReply OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;               \
virtual FReply OnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;                           \
virtual FReply OnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;                             \
virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;                  \
virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;                      \
virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;                  \
virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;                   \
virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;                                                \
public:

/**
 * Place this macro in the .cpp file for the class you put HEART_SLATE_INPUT_LINKER_HEADER in.
 */
#define HEART_SLATE_INPUT_LINKER_BODY(type)\
FReply type::OnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)                              \
{                                                                                                                      \
	auto&& Reply = Input::LinkOnMouseWheel<SWidget>(SharedThis(this), InGeometry, InMouseEvent);                       \
	return Reply.IsEventHandled() ? Reply : Super::OnMouseWheel(InGeometry, InMouseEvent);                             \
}                                                                                                                      \
																													   \
FReply type::OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)                         \
{                                                                                                                      \
	auto&& Reply = Input::LinkOnMouseButtonDown<SWidget>(SharedThis(this), InGeometry, InMouseEvent);                  \
	return Reply.IsEventHandled() ? Reply : Super::OnMouseButtonDown(InGeometry, InMouseEvent);                        \
}                                                                                                                      \
																													   \
FReply type::OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)                           \
{                                                                                                                      \
	auto&& Reply = Input::LinkOnMouseButtonUp<SWidget>(SharedThis(this), InGeometry, InMouseEvent);                    \
	return Reply.IsEventHandled() ? Reply : Super::OnMouseButtonUp(InGeometry, InMouseEvent);                          \
}                                                                                                                      \
																													   \
FReply type::OnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)                                       \
{                                                                                                                      \
	auto&& Reply = Input::LinkOnKeyDown<SWidget>(SharedThis(this), InGeometry, InKeyEvent);                            \
	return Reply.IsEventHandled() ? Reply : Super::OnKeyDown(InGeometry, InKeyEvent);                                  \
}                                                                                                                      \
																													   \
FReply type::OnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)                                         \
{                                                                                                                      \
	auto&& Reply = Input::LinkOnKeyUp<SWidget>(SharedThis(this), InGeometry, InKeyEvent);                              \
	return Reply.IsEventHandled() ? Reply : Super::OnKeyUp(InGeometry, InKeyEvent);                                    \
}                                                                                                                      \
																													   \
FReply type::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)                              \
{                                                                                                                      \
	auto&& Reply = Input::LinkOnDragDetected<SWidget>(SharedThis(this), MyGeometry, MouseEvent);                       \
	return Reply.IsEventHandled() ? Reply : Super::OnDragDetected(MyGeometry, MouseEvent);                             \
}                                                                                                                      \
																													   \
FReply type::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)                                  \
{                                                                                                                      \
	auto&& Reply = Input::LinkOnDrop<SWidget, FReply>(SharedThis(this), MyGeometry, DragDropEvent);                    \
	return Reply.IsEventHandled() ? Reply : Super::OnDrop(MyGeometry, DragDropEvent);                                  \
}                                                                                                                      \
																													   \
FReply type::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)                              \
{                                                                                                                      \
	auto&& Reply = Input::LinkOnDragOver<SWidget, FReply>(SharedThis(this), MyGeometry, DragDropEvent);                \
	return Reply.IsEventHandled() ? Reply : Super::OnDragOver(MyGeometry, DragDropEvent);                              \
}                                                                                                                      \
																													   \
void type::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)                               \
{                                                                                                                      \
	Super::OnDragEnter(MyGeometry, DragDropEvent);                                                                     \
	Input::LinkOnDragEnter<SWidget>(SharedThis(this), MyGeometry, DragDropEvent);                                      \
}                                                                                                                      \
																													   \
void type::OnDragLeave(const FDragDropEvent& DragDropEvent)                                                            \
{                                                                                                                      \
	Super::OnDragLeave(DragDropEvent);                                                                                 \
	Input::LinkOnDragLeave<SWidget>(SharedThis(this), DragDropEvent);                                                  \
}