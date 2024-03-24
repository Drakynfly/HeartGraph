// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Input/HeartInputTypes.h"
#include "Input/HeartInputTrip.h"
#include "HeartInputLinkerBase.generated.h"

struct FHeartBoundInput;
struct FHeartManualEvent;

USTRUCT(BlueprintType)
struct FHeartManualInputQueryResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ManualInputQueryResult")
	FName Key;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ManualInputQueryResult")
	FText Description;
};

UCLASS()
class HEARTCORE_API UHeartInputLinkerBase : public UObject
{
	GENERATED_BODY()

public:
	void BindInputCallback(const Heart::Input::FInputTrip& Trip, const Heart::Input::FConditionalCallback& InputCallback);
	void UnbindInputCallback(const Heart::Input::FInputTrip& Trip);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetInputLinker")
	void AddBindings(const TArray<FHeartBoundInput>& Bindings);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetInputLinker")
	void RemoveBindings(const TArray<FHeartBoundInput>& Bindings);

protected:
	// Input trips that fire a delegate.
	TMultiMap<Heart::Input::FInputTrip, Heart::Input::FConditionalCallback> InputCallbackMappings;
};

namespace Heart::Input
{
	template <
		typename TWidget
		UE_REQUIRES(TLinkerType<TWidget>::Supported)
	>
	static FReply LinkOnMouseWheel(typename TLinkerType<TWidget>::FValueType Widget, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = TLinkerType<TWidget>::FindLinker(Widget))
		{
			FReply BindingReply = Linker->HandleOnMouseWheel(Widget, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return FReply::Unhandled();
	}

	template <
		typename TWidget
		UE_REQUIRES(TLinkerType<TWidget>::Supported)
	>
	static FReply LinkOnMouseButtonDown(typename TLinkerType<TWidget>::FValueType Widget, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = TLinkerType<TWidget>::FindLinker(Widget))
		{
			FReply BindingReply = Linker->HandleOnMouseButtonDown(Widget, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return FReply::Unhandled();
	}

	template <
		typename TWidget
		UE_REQUIRES(TLinkerType<TWidget>::Supported)
	>
	static FReply LinkOnMouseButtonUp(typename TLinkerType<TWidget>::FValueType Widget, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = TLinkerType<TWidget>::FindLinker(Widget))
		{
			FReply BindingReply = Linker->HandleOnMouseButtonUp(Widget, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return FReply::Unhandled();
	}

	template <
		typename TWidget
		UE_REQUIRES(TLinkerType<TWidget>::Supported)
	>
	static FReply LinkOnKeyDown(typename TLinkerType<TWidget>::FValueType Widget, const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
	{
		if (auto&& Linker = TLinkerType<TWidget>::FindLinker(Widget))
		{
			FReply BindingReply = Linker->HandleOnKeyDown(Widget, InGeometry, InKeyEvent);

			if (BindingReply.IsEventHandled())
	        {
        		return BindingReply;
	        }
		}

		return FReply::Unhandled();
	}

	template <
		typename TWidget
		UE_REQUIRES(TLinkerType<TWidget>::Supported)
	>
	static FReply LinkOnKeyUp(typename TLinkerType<TWidget>::FValueType Widget, const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
	{
		if (auto&& Linker = TLinkerType<TWidget>::FindLinker(Widget))
		{
			FReply BindingReply = Linker->HandleOnKeyUp(Widget, InGeometry, InKeyEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return FReply::Unhandled();
	}

	template <
		typename TWidget
		UE_REQUIRES(TLinkerType<TWidget>::Supported)
	>
	static TOptional<typename TLinkerType<TWidget>::FDDOType> LinkOnDragDetected(typename TLinkerType<TWidget>::FValueType Widget, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = TLinkerType<TWidget>::FindLinker(Widget))
		{
			if (auto&& LinkerOperation = Linker->HandleOnDragDetected(Widget, InGeometry, InMouseEvent))
			{
				return LinkerOperation;
			}
		}

		return {};
	}

	template <
		typename TWidget
		UE_REQUIRES(TLinkerType<TWidget>::Supported)
	>
	static bool LinkOnDrop(typename TLinkerType<TWidget>::FValueType Widget, const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
						   typename TLinkerType<TWidget>::FDDOType InOperation)
	{
		if (auto&& Linker = TLinkerType<TWidget>::FindLinker(Widget))
		{
			if (Linker->HandleNativeOnDrop(Widget, InGeometry, InDragDropEvent, InOperation))
			{
				return true;
			}
		}

		return false;
	}

	template <
		typename TWidget
		UE_REQUIRES(TLinkerType<TWidget>::Supported)
	>
	static bool LinkOnDragOver(typename TLinkerType<TWidget>::FValueType Widget, const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
							   typename TLinkerType<TWidget>::FDDOType InOperation)
	{
		if (auto&& Linker = TLinkerType<TWidget>::FindLinker(Widget))
		{
			if (Linker->HandleNativeOnDragOver(Widget, InGeometry, InDragDropEvent, InOperation))
			{
				return true;
			}
		}

		return false;
	}

	template <
		typename TWidget
		UE_REQUIRES(TLinkerType<TWidget>::Supported)
	>
	static void LinkOnDragEnter(typename TLinkerType<TWidget>::FValueType Widget, const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
								typename TLinkerType<TWidget>::FDDOType InOperation)
	{
		if (auto&& Linker = TLinkerType<TWidget>::FindLinker(Widget))
		{
			Linker->HandleNativeOnDragEnter(Widget, InGeometry, InDragDropEvent, InOperation);
		}
	}

	template <
		typename TWidget
		UE_REQUIRES(TLinkerType<TWidget>::Supported)
	>
	static void LinkOnDragLeave(typename TLinkerType<TWidget>::FValueType Widget, const FDragDropEvent& InDragDropEvent,
								typename TLinkerType<TWidget>::FDDOType InOperation)
	{
		if (auto&& Linker = TLinkerType<TWidget>::FindLinker(Widget))
		{
			Linker->HandleNativeOnDragLeave(Widget, InDragDropEvent, InOperation);
		}
	}

	template <
		typename TWidget
		UE_REQUIRES(TLinkerType<TWidget>::Supported)
	>
	static void LinkOnDragCancelled(typename TLinkerType<TWidget>::FValueType Widget, const FDragDropEvent& InDragDropEvent,
									typename TLinkerType<TWidget>::FDDOType InOperation)
	{
		if (auto&& Linker = TLinkerType<TWidget>::FindLinker(Widget))
		{
			Linker->HandleNativeOnDragCancelled(Widget, InDragDropEvent, InOperation);
		}
	}
}