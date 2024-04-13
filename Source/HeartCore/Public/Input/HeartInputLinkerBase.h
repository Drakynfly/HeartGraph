// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Input/HeartInputTypes.h"
#include "Input/HeartInputTrip.h"
#include "HeartInputLinkerBase.generated.h"

class UHeartInputLinkerBase;
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

namespace Heart::Input
{
	class HEARTCORE_API FCallbackQuery
	{
	public:
		FCallbackQuery(const UHeartInputLinkerBase* Linker, const FHeartInputTrip& Trip);

		FCallbackQuery& Sort();

		FCallbackQuery& ForEachWithBreak(const UObject* Target, const TFunctionRef<bool(const FHeartSortableInputCallback&)>& Predicate);

	private:
		TArray<FHeartSortableInputCallback> Callbacks;
	};
}

UCLASS()
class HEARTCORE_API UHeartInputLinkerBase : public UObject
{
	GENERATED_BODY()

	friend Heart::Input::FCallbackQuery;

protected:
	FHeartEvent QuickTryCallbacks(const FHeartInputTrip& Trip, UObject* Target, const FHeartInputActivation& Activation);

public:
	void BindInputCallback(const FHeartInputTrip& Trip, const FHeartSortableInputCallback& InputCallback);
	void UnbindInputCallback(const FHeartInputTrip& Trip);

	// Custom input
	virtual FHeartEvent HandleManualInput(UObject* Target, FName Key, const FHeartManualEvent& Activation);
	TArray<FHeartManualInputQueryResult> QueryManualTriggers(const UObject* Target) const;

	Heart::Input::FCallbackQuery Query(const FHeartInputTrip& Trip) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|InputLinker")
	void AddBindings(const TArray<FHeartBoundInput>& Bindings);

	UFUNCTION(BlueprintCallable, Category = "Heart|InputLinker")
	void RemoveBindings(const TArray<FHeartBoundInput>& Bindings);

protected:
	// Input trips that fire a delegate.
	UPROPERTY()
	TMap<FHeartInputTrip, FHeartSortableCallbackList> InputCallbackMappings;
};

namespace Heart::Input
{
	template <
		typename TTarget
		UE_REQUIRES(THeartInputLinkerType<TTarget>::Supported)
	>
	static typename THeartInputLinkerType<TTarget>::FReplyType LinkOnMouseWheel(typename THeartInputLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = THeartInputLinkerType<TTarget>::FindLinker(Target))
		{
			auto BindingReply = Linker->HandleOnMouseWheel(Target, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return THeartInputLinkerType<TTarget>::NoReply();
	}

	template <
		typename TTarget
		UE_REQUIRES(THeartInputLinkerType<TTarget>::Supported)
	>
	static typename THeartInputLinkerType<TTarget>::FReplyType LinkOnMouseButtonDown(typename THeartInputLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = THeartInputLinkerType<TTarget>::FindLinker(Target))
		{
			auto BindingReply = Linker->HandleOnMouseButtonDown(Target, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return THeartInputLinkerType<TTarget>::NoReply();
	}

	template <
		typename TTarget
		UE_REQUIRES(THeartInputLinkerType<TTarget>::Supported)
	>
	static typename THeartInputLinkerType<TTarget>::FReplyType LinkOnMouseButtonUp(typename THeartInputLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = THeartInputLinkerType<TTarget>::FindLinker(Target))
		{
			auto BindingReply = Linker->HandleOnMouseButtonUp(Target, InGeometry, InMouseEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return THeartInputLinkerType<TTarget>::NoReply();
	}

	template <
		typename TTarget
		UE_REQUIRES(THeartInputLinkerType<TTarget>::Supported)
	>
	static typename THeartInputLinkerType<TTarget>::FReplyType LinkOnKeyDown(typename THeartInputLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
	{
		if (auto&& Linker = THeartInputLinkerType<TTarget>::FindLinker(Target))
		{
			return Linker->HandleOnKeyDown(Target, InGeometry, InKeyEvent);
		}

		return THeartInputLinkerType<TTarget>::NoReply();
	}

	template <
		typename TTarget
		UE_REQUIRES(THeartInputLinkerType<TTarget>::Supported)
	>
	static typename THeartInputLinkerType<TTarget>::FReplyType LinkOnKeyUp(typename THeartInputLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
	{
		if (auto&& Linker = THeartInputLinkerType<TTarget>::FindLinker(Target))
		{
			auto BindingReply = Linker->HandleOnKeyUp(Target, InGeometry, InKeyEvent);

			if (BindingReply.IsEventHandled())
			{
				return BindingReply;
			}
		}

		return THeartInputLinkerType<TTarget>::NoReply();
	}

	template <
		typename TTarget
		UE_REQUIRES(THeartInputLinkerType<TTarget>::Supported)
	>
	static typename THeartInputLinkerType<TTarget>::FDDOType LinkOnDragDetected(typename THeartInputLinkerType<TTarget>::FValueType Target, const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		if (auto&& Linker = THeartInputLinkerType<TTarget>::FindLinker(Target))
		{
			return Linker->HandleOnDragDetected(Target, InGeometry, InMouseEvent);
		}

		if constexpr (std::is_same_v<typename THeartInputLinkerType<TTarget>::FDDOType, FReply>)
		{
			return THeartInputLinkerType<TTarget>::NoReply();
		}
		else return {};
	}

	template <
		typename TTarget,
		typename TRetVal,
		typename... TArgs
		UE_REQUIRES(THeartInputLinkerType<TTarget>::Supported)
	>
	static TRetVal LinkOnDrop(typename THeartInputLinkerType<TTarget>::FValueType Target, TArgs... Args)
	{
		if (auto&& Linker = THeartInputLinkerType<TTarget>::FindLinker(Target))
		{
			return Linker->HandleOnDrop(Target, Args...);
		}

		if constexpr (std::is_same_v<typename THeartInputLinkerType<TTarget>::FDDOType, FReply>)
		{
			return THeartInputLinkerType<TTarget>::NoReply();
		}
		else return {};
	}

	template <
		typename TTarget,
		typename TRetVal,
		typename... TArgs
		UE_REQUIRES(THeartInputLinkerType<TTarget>::Supported)
	>
	static TRetVal LinkOnDragOver(typename THeartInputLinkerType<TTarget>::FValueType Target, TArgs... Args)
	{
		if (auto&& Linker = THeartInputLinkerType<TTarget>::FindLinker(Target))
		{
			return Linker->HandleOnDragOver(Target, Args...);
		}

		if constexpr (std::is_same_v<typename THeartInputLinkerType<TTarget>::FDDOType, FReply>)
		{
			return THeartInputLinkerType<TTarget>::NoReply();
		}
		else return {};
	}

	template <
		typename TTarget,
		typename... TArgs
		UE_REQUIRES(THeartInputLinkerType<TTarget>::Supported)
	>
	static void LinkOnDragEnter(typename THeartInputLinkerType<TTarget>::FValueType Target, TArgs... Args)
	{
		if (auto&& Linker = THeartInputLinkerType<TTarget>::FindLinker(Target))
		{
			Linker->HandleOnDragEnter(Target, Args...);
		}
	}

	template <
		typename TTarget,
		typename... TArgs
		UE_REQUIRES(THeartInputLinkerType<TTarget>::Supported)
	>
	static void LinkOnDragLeave(typename THeartInputLinkerType<TTarget>::FValueType Target, TArgs... Args)
	{
		if (auto&& Linker = THeartInputLinkerType<TTarget>::FindLinker(Target))
		{
			Linker->HandleOnDragLeave(Target, Args...);
		}
	}

	template <
		typename TTarget,
		typename... TArgs
		UE_REQUIRES(THeartInputLinkerType<TTarget>::Supported)
	>
	static void LinkOnDragCancelled(typename THeartInputLinkerType<TTarget>::FValueType Target, TArgs... Args)
	{
		if (auto&& Linker = THeartInputLinkerType<TTarget>::FindLinker(Target))
		{
			Linker->HandleOnDragCancelled(Target, Args...);
		}
	}
}