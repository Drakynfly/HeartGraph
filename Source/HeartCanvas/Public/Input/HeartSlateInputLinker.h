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

namespace Heart::Input
{
	template <>
	struct TLinkerType<SWidget>
	{
		static constexpr bool Supported = true;

		using FReplyType = FReply;
		using FValueType = const TSharedRef<SWidget>&;
		using FDDOType = FReply;

		template <typename T>
		static FORCEINLINE T DefaultReply()
		{
			if constexpr (std::is_same_v<T, FReply>)
			{
				return FReply::Unhandled();
			}
			else if constexpr (std::is_same_v<T, void>)
			{
				return;
			}
			else return {};
		}

		HEARTCANVAS_API static UHeartSlateInputLinker* FindLinker(const TSharedRef<SWidget>& Widget);
	};
}

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

#define HEART_SLATE_INPUT_INVOKE_REPLY(Prototype, InputType)                                                           \
FReply ThisClass::Prototype(const FGeometry& InGeometry, const InputType& _InputEvent)                                 \
{                                                                                                                      \
	auto&& Reply = Heart::Input::InvokeLinker<SWidget>(SharedThis(this),                                               \
		&UHeartSlateInputLinker::Handle##Prototype, InGeometry, _InputEvent);                                          \
	return Reply.IsEventHandled() ? Reply : Super::Prototype(InGeometry, _InputEvent);                                 \
}

/**
 * Place this macro in the .cpp file for the class you put HEART_SLATE_INPUT_LINKER_HEADER in.
 */
#define HEART_SLATE_INPUT_LINKER_BODY(type)                                                                            \
	using ThisClass = type;                                                                                            \
	HEART_SLATE_INPUT_INVOKE_REPLY(OnMouseWheel, FPointerEvent)                                                        \
	HEART_SLATE_INPUT_INVOKE_REPLY(OnMouseButtonDown, FPointerEvent)                                                   \
	HEART_SLATE_INPUT_INVOKE_REPLY(OnMouseButtonUp, FPointerEvent)                                                     \
	HEART_SLATE_INPUT_INVOKE_REPLY(OnKeyDown, FKeyEvent)                                                               \
	HEART_SLATE_INPUT_INVOKE_REPLY(OnKeyUp, FKeyEvent)                                                                 \
	HEART_SLATE_INPUT_INVOKE_REPLY(OnDragDetected, FPointerEvent)                                                      \
	HEART_SLATE_INPUT_INVOKE_REPLY(OnDrop, FDragDropEvent)                                                             \
	HEART_SLATE_INPUT_INVOKE_REPLY(OnDragOver, FDragDropEvent)                                                         \
																													   \
void ThisClass::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)                               \
{                                                                                                                      \
	Super::OnDragEnter(MyGeometry, DragDropEvent);                                                                     \
	Heart::Input::LinkOnDragEnter<SWidget>(SharedThis(this), MyGeometry, DragDropEvent);                               \
}                                                                                                                      \
																													   \
void ThisClass::OnDragLeave(const FDragDropEvent& DragDropEvent)                                                            \
{                                                                                                                      \
	Super::OnDragLeave(DragDropEvent);                                                                                 \
	Heart::Input::LinkOnDragLeave<SWidget>(SharedThis(this), DragDropEvent);                                           \
}