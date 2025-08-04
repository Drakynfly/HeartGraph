// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Blueprint/DragDropOperation.h"
#include "Input/DragAndDrop.h"
#include "Input/HeartEvent.h"
#include "Input/Reply.h"
#include "HeartDragDropOperation.generated.h"

namespace Heart
{
	// A heart native drag drop operation
	class FNativeDragDropOperation : public FDragDropOperation
	{
	public:
		DRAG_DROP_OPERATOR_TYPE(FNativeDragDropOperation, FDragDropOperation)

		virtual bool SetupDragDropOperation() PURE_VIRTUAL(UHeartDragDropOperation::SetupDragDropOperation, return false; )

		virtual FReply OnDropOnWidget(const TSharedRef<SWidget>& Widget) { return FReply::Handled(); }
		virtual FReply OnHoverWidget(const TSharedRef<SWidget>& Widget) { return FReply::Unhandled(); }

		// The widget that created this drag drop operation
		TSharedPtr<SWidget> SummonedBy;
	};
}

/**
 * A heart UMG drag drop operation
 */
UCLASS(Abstract)
class HEARTCANVAS_API UHeartDragDropOperation : public UDragDropOperation, public IHeartDeferredEventHandler
{
	GENERATED_BODY()

public:
	virtual bool CanRunOnWidget(const UWidget* Widget) const { return true; }

	virtual bool SetupDragDropOperation() PURE_VIRTUAL(UHeartDragDropOperation::SetupDragDropOperation, return false; )

	virtual bool OnDropOnWidget(UWidget* Widget) { return true; }

	virtual bool OnHoverWidget(UWidget* Widget) { return false; }

	// The widget that created this drag drop operation
	UPROPERTY()
	TObjectPtr<UWidget> SummonedBy;
};