// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

struct FHeartEvent;
struct FHeartInputActivation;

namespace Heart::Input
{
	/** Unified storage for both native and dynamic delegates with any signature  */
	// Based on TEnhancedInputUnifiedDelegate
	template<typename TSignature>
	struct TUnifiedDelegate
	{
	private:
		/** Holds the delegate to call. */
		TSharedPtr<TSignature> Delegate;

	public:
		bool IsBound() const
		{
			return Delegate.IsValid() && Delegate->IsBound();
		}

		void Unbind()
		{
			if (Delegate)
			{
				Delegate->Unbind();
			}
		}

		/** Binds a native delegate, hidden for script delegates */
		template<	typename UserClass,
					typename TSig = TSignature,
					typename... TVars>
		void BindDelegate(UserClass* Object, typename TSig::template TMethodPtr<UserClass, TVars...> Func, TVars... Vars)
		{
			Unbind();
			Delegate = MakeShared<TSig>(TSig::CreateUObject(Object, Func, Vars...));
		}

		/** Binds a native delegate, hidden for script delegates */
		template<	typename UserClass,
					typename TSig = TSignature,
					typename... TVars>
		void BindDelegate(const UserClass* Object, typename TSig::template TConstMethodPtr<UserClass, TVars...> Func, TVars... Vars)
		{
			Unbind();
			Delegate = MakeShared<TSig>(TSig::CreateUObject(Object, Func, Vars...));
		}

		/** Binds a script delegate on an arbitrary UObject, hidden for native delegates */
		template<
			typename TSig = TSignature
			UE_REQUIRES(TIsDerivedFrom<TSig, FScriptDelegate>::IsDerived || TIsDerivedFrom<TSig, FMulticastScriptDelegate>::IsDerived)
		>
		void BindDelegate(UObject* Object, const FName FuncName)
		{
			Unbind();
			Delegate = MakeShared<TSig>();
			Delegate->BindUFunction(Object, FuncName);
		}

		template<typename TSig = TSignature>
		TSig& MakeDelegate()
		{
			Unbind();
			Delegate = MakeShared<TSig>();
			return *Delegate;
		}

		template<typename... TArgs>
		auto Execute(TArgs... Args) const
		{
			return Delegate->Execute(Args...);
		}
	};

	struct FDelegateBase
	{
	};

	template <typename TSignature>
	struct TSpecifiedDelegate : FDelegateBase
	{
		using FSignature = TSignature;

		TSpecifiedDelegate() {}

		template<typename... TArgs>
		TSpecifiedDelegate(TArgs... Args)
		{
			Delegate.BindDelegate(Args...);
		}

		bool IsBound()
		{
			return Delegate.IsBound();
		}

		template<typename... TArgs>
		auto Execute(TArgs... Args) const
		{
			return Delegate.Execute(Args...);
		}

	private:
		TUnifiedDelegate<FSignature> Delegate;
	};

	template <typename T>
	struct TLinkerType
	{
		static constexpr bool Supported = false;
	};

	using FDescriptionDelegate = TDelegate<FText(const UObject*)>;
	using FConditionDelegate = TDelegate<bool(const UObject*)>;
	using FHandlerDelegate = TDelegate<FHeartEvent(UObject*, const FHeartInputActivation&)>;

	enum EExecutionOrder
	{
		None, // Blank layer. Do not use.
		Event, // Default layer. Handlers can capture input or bubble it.
		Listener, // Interception layer. Handlers can intercept, but cannot stop it from bubbling.
	};

	struct FConditionalInputBase
	{
		// Callback to retrieve a text description of the action
		const FDescriptionDelegate Description;

		// Callback to determine if the context of the trigger is valid for executing the action
		const FConditionDelegate Condition;

		// Determines the order that callback handler run in, and whether they bubble the input callstack
		const EExecutionOrder Priority = None;

		friend bool operator<(const FConditionalInputBase& A, const FConditionalInputBase& B)
		{
			// Sort in reverse. Higher priorities should be ordered first, lower after.
			return A.Priority > B.Priority;
		}
	};

	struct FConditionalCallback : FConditionalInputBase
	{
		FConditionalCallback(
			const FHandlerDelegate& Handler,
			const FDescriptionDelegate& Description,
			const FConditionDelegate& Condition,
			const EExecutionOrder Priority)
		  : FConditionalInputBase(Description, Condition, Priority), Handler(Handler) {}

		// Callback to execute the event
		const FHandlerDelegate Handler;
	};

	struct FConditionalCallback_DDO : FConditionalInputBase
	{
		FConditionalCallback_DDO(
			const TSharedPtr<FDelegateBase>& Handler,
			const FDescriptionDelegate& Description,
			const FConditionDelegate& Condition,
			const EExecutionOrder Priority)
		  : FConditionalInputBase(Description, Condition, Priority), Handler(Handler) {}

		// Callback to begin a DDO
		const TSharedPtr<FDelegateBase> Handler;
	};
}