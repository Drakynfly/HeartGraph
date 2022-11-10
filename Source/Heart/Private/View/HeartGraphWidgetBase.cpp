// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "View/HeartGraphWidgetBase.h"

#include "UI/HeartWidgetInputLinkerRedirector.h"

FReply UHeartGraphWidgetBase::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (auto&& Linker = GetInputLinker())
	{
		FReply BindingReply = Linker->HandleOnMouseWheel(this, InMouseEvent);

		if (BindingReply.IsEventHandled())
		{
			return BindingReply;
		}
	}

	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}

FReply UHeartGraphWidgetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (auto&& Linker = GetInputLinker())
	{
		FReply BindingReply = Linker->HandleOnMouseButtonDown(this, InMouseEvent);

		if (BindingReply.IsEventHandled())
		{
			return BindingReply;
		}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UHeartGraphWidgetBase::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (auto&& Linker = GetInputLinker())
	{
		FReply BindingReply = Linker->HandleOnMouseButtonUp(this, InMouseEvent);

		if (BindingReply.IsEventHandled())
		{
			return BindingReply;
		}
	}

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UHeartGraphWidgetBase::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (auto&& Linker = GetInputLinker())
	{
		FReply BindingReply = Linker->HandleOnKeyDown(this, InKeyEvent);

		if (BindingReply.IsEventHandled())
        {
        	return BindingReply;
        }
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UHeartGraphWidgetBase::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (auto&& Linker = GetInputLinker())
	{
		FReply BindingReply = Linker->HandleOnKeyUp(this, InKeyEvent);

		if (BindingReply.IsEventHandled())
		{
			return BindingReply;
		}
	}

	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

void UHeartGraphWidgetBase::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                                 UDragDropOperation*& OutOperation)
{
	if (auto&& Linker = GetInputLinker())
	{
		if (auto&& LinkerOperation = Linker->HandleOnDragDetected(this, InMouseEvent))
		{
			OutOperation = LinkerOperation;
			return;
		}
	}

	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool UHeartGraphWidgetBase::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if (auto&& Linker = GetInputLinker())
	{
		if (Linker->HandleNativeOnDrop(this, InDragDropEvent, InOperation))
		{
			return true;
		}
	}

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

bool UHeartGraphWidgetBase::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if (auto&& Linker = GetInputLinker())
	{
		if (Linker->HandleNativeOnDragOver(this, InDragDropEvent, InOperation))
		{
			return true;
		}
	}

	return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}
/*
void UHeartGraphWidgetBase::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
}

void UHeartGraphWidgetBase::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
}

void UHeartGraphWidgetBase::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}
*/

UHeartWidgetInputLinker* UHeartGraphWidgetBase::GetInputLinker() const
{
	if (auto&& Redirector = Cast<IHeartWidgetInputLinkerRedirector>(this))
	{
		return Redirector->ResolveLinker();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Child of UHeartGraphWidgetBase did not implement IHeartWidgetInputLinkerRedirector! Linker input will not function."))
		return nullptr;
	}
}
