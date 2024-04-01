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

protected:
	FHeartEvent TryCallbacks(const Heart::Input::FInputTrip& Trip, UObject* Target, const FHeartInputActivation& Activation);

public:
	void BindInputCallback(const Heart::Input::FInputTrip& Trip, const TSharedPtr<const Heart::Input::FConditionalCallback>& InputCallback);
	void UnbindInputCallback(const Heart::Input::FInputTrip& Trip);

	// Custom input
	virtual FHeartEvent HandleManualInput(UObject* Target, FName Key, const FHeartManualEvent& Activation);
	TArray<FHeartManualInputQueryResult> QueryManualTriggers(const UObject* Target) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|InputLinker")
	void AddBindings(const TArray<FHeartBoundInput>& Bindings);

	UFUNCTION(BlueprintCallable, Category = "Heart|InputLinker")
	void RemoveBindings(const TArray<FHeartBoundInput>& Bindings);

protected:
	// Input trips that fire a delegate.
	TMultiMap<Heart::Input::FInputTrip, TSharedPtr<const Heart::Input::FConditionalCallback>> InputCallbackMappings;
};

namespace Heart::Input
{
	template <
		typename TTarget
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static typename TLinkerType<TTarget>::FReplyType LinkOnMouseWheel(typename TLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			auto BindingReply = Linker->HandleOnMouseWheel(Target, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return TLinkerType<TTarget>::NoReply();
	}

	template <
		typename TTarget
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static typename TLinkerType<TTarget>::FReplyType LinkOnMouseButtonDown(typename TLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			auto BindingReply = Linker->HandleOnMouseButtonDown(Target, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return TLinkerType<TTarget>::NoReply();
	}

	template <
		typename TTarget
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static typename TLinkerType<TTarget>::FReplyType LinkOnMouseButtonUp(typename TLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			auto BindingReply = Linker->HandleOnMouseButtonUp(Target, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return TLinkerType<TTarget>::NoReply();
	}

	template <
		typename TTarget
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static typename TLinkerType<TTarget>::FReplyType LinkOnKeyDown(typename TLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
	{
		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			auto BindingReply = Linker->HandleOnKeyDown(Target, InGeometry, InKeyEvent);

			if (BindingReply.IsEventHandled())
	        {
        		return BindingReply;
	        }
		}

		return TLinkerType<TTarget>::NoReply();
	}

	template <
		typename TTarget
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static typename TLinkerType<TTarget>::FReplyType LinkOnKeyUp(typename TLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
	{
		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			auto BindingReply = Linker->HandleOnKeyUp(Target, InGeometry, InKeyEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return TLinkerType<TTarget>::NoReply();
	}

	template <
		typename TTarget
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static TOptional<typename TLinkerType<TTarget>::FDDOType> LinkOnDragDetected(typename TLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			if (auto&& LinkerOperation = Linker->HandleOnDragDetected(Target, InGeometry, InMouseEvent))
			{
				return LinkerOperation;
			}
		}

		return {};
	}

	template <
		typename TTarget
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static bool LinkOnDrop(typename TLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
						   typename TLinkerType<TTarget>::FDDOType InOperation)
	{
		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			if (Linker->HandleNativeOnDrop(Target, InGeometry, InDragDropEvent, InOperation))
			{
				return true;
			}
		}

		return false;
	}

	template <
		typename TTarget
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static bool LinkOnDragOver(typename TLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
							   typename TLinkerType<TTarget>::FDDOType InOperation)
	{
		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			if (Linker->HandleNativeOnDragOver(Target, InGeometry, InDragDropEvent, InOperation))
			{
				return true;
			}
		}

		return false;
	}

	template <
		typename TTarget
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static void LinkOnDragEnter(typename TLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
								typename TLinkerType<TTarget>::FDDOType InOperation)
	{
		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			Linker->HandleNativeOnDragEnter(Target, InGeometry, InDragDropEvent, InOperation);
		}
	}

	template <
		typename TTarget
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static void LinkOnDragLeave(typename TLinkerType<TTarget>::FValueType Target, const FDragDropEvent& InDragDropEvent,
								typename TLinkerType<TTarget>::FDDOType InOperation)
	{
		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			Linker->HandleNativeOnDragLeave(Target, InDragDropEvent, InOperation);
		}
	}

	template <
		typename TTarget
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static void LinkOnDragCancelled(typename TLinkerType<TTarget>::FValueType Target, const FDragDropEvent& InDragDropEvent,
									typename TLinkerType<TTarget>::FDDOType InOperation)
	{
		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			Linker->HandleNativeOnDragCancelled(Target, InDragDropEvent, InOperation);
		}
	}
}