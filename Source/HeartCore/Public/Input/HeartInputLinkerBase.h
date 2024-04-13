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
	// @todo this is a stub. this should eventually be refactored to be a Heart::Query result type.
	class HEARTCORE_API FCallbackQuery
	{
	public:
		FCallbackQuery(const UHeartInputLinkerBase* Linker, const FInputTrip& Trip);

		FCallbackQuery& Sort();

		FCallbackQuery& ForEachWithBreak(const UObject* Target, const TFunctionRef<bool(const FSortableCallback&)>& Predicate);

	private:
		TArray<const FSortableCallback*> Callbacks;
	};
}

UCLASS()
class HEARTCORE_API UHeartInputLinkerBase : public UObject
{
	GENERATED_BODY()

	friend Heart::Input::FCallbackQuery;

protected:
	FHeartEvent QuickTryCallbacks(const Heart::Input::FInputTrip& Trip, UObject* Target, const FHeartInputActivation& Activation);

public:
	void BindInputCallback(const Heart::Input::FInputTrip& Trip, const Heart::Input::FSortableCallback& InputCallback);
	void UnbindInputCallback(const Heart::Input::FInputTrip& Trip);

	// Custom input
	virtual FHeartEvent HandleManualInput(UObject* Target, FName Key, const FHeartManualEvent& Activation);
	TArray<FHeartManualInputQueryResult> QueryManualTriggers(const UObject* Target) const;

	Heart::Input::FCallbackQuery Query(const Heart::Input::FInputTrip& Trip) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|InputLinker")
	void AddBindings(const TArray<FHeartBoundInput>& Bindings);

	UFUNCTION(BlueprintCallable, Category = "Heart|InputLinker")
	void RemoveBindings(const TArray<FHeartBoundInput>& Bindings);

protected:
	// Input trips that fire a delegate.
	TMultiMap<Heart::Input::FInputTrip, const Heart::Input::FSortableCallback> InputCallbackMappings;
};

namespace Heart::Input
{
	/*
	 * This template takes a function reference to a linker member function, and calls it if a linker for the target can
	 * be found.
	 */
	template <
		typename TTarget,
		typename TMember,
		typename... TArgs
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static auto InvokeLinker(typename TLinkerType<TTarget>::FValueType Target, TMember&& Member, const TArgs&... Args)
	{
		using RetType = typename TLinkerType<TTarget>::FReplyType;

		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			return Invoke(Member, Linker, Target, Args...);
		}

		return TLinkerType<TTarget>::template DefaultReply<RetType>();
	}

	template <
		typename TTarget,
		typename... TArgs
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static typename TLinkerType<TTarget>::FDDOType LinkOnDragDetected(typename TLinkerType<TTarget>::FValueType Target, TArgs... Args)
	{
		using RetType = typename TLinkerType<TTarget>::FDDOType;

		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			return Linker->HandleOnDragDetected(Target, Args...);
		}

		return TLinkerType<TTarget>::template DefaultReply<RetType>();
	}

	template <
		typename TTarget,
		typename TRetVal,
		typename... TArgs
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static TRetVal LinkOnDrop(typename TLinkerType<TTarget>::FValueType Target, TArgs... Args)
	{
		using RetType = TRetVal;

		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			return Linker->HandleOnDrop(Target, Args...);
		}

		return TLinkerType<TTarget>::template DefaultReply<RetType>();
	}

	template <
		typename TTarget,
		typename TRetVal,
		typename... TArgs
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static TRetVal LinkOnDragOver(typename TLinkerType<TTarget>::FValueType Target, TArgs... Args)
	{
		using RetType = TRetVal;

		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			return Linker->HandleOnDragOver(Target, Args...);
		}

		return TLinkerType<TTarget>::template DefaultReply<RetType>();
	}

	template <
		typename TTarget,
		typename... TArgs
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static void LinkOnDragEnter(typename TLinkerType<TTarget>::FValueType Target, TArgs... Args)
	{
		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			Linker->HandleOnDragEnter(Target, Args...);
		}
	}

	template <
		typename TTarget,
		typename... TArgs
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static void LinkOnDragLeave(typename TLinkerType<TTarget>::FValueType Target, TArgs... Args)
	{
		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			Linker->HandleOnDragLeave(Target, Args...);
		}
	}

	template <
		typename TTarget,
		typename... TArgs
		UE_REQUIRES(TLinkerType<TTarget>::Supported)
	>
	static void LinkOnDragCancelled(typename TLinkerType<TTarget>::FValueType Target, TArgs... Args)
	{
		if (auto&& Linker = TLinkerType<TTarget>::FindLinker(Target))
		{
			Linker->HandleOnDragCancelled(Target, Args...);
		}
	}
}