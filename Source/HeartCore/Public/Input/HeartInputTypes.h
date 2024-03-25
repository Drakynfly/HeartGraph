// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

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

	enum EHeartInputLayer
	{
		None, // Blank layer. Do not use.
		Event, // Default layer. Events may return FReply::Handled
		Listener, // Interception layer. Listeners always return FReply::Unhandled
	};

	struct FConditionalInputBase
	{
		// Callback to retrieve a text description of the action
		const TSharedPtr<FDelegateBase> Description;

		// Callback to determine if the context of the trigger is valid for executing the action
		const TSharedPtr<FDelegateBase> Condition;

		// Input layers determine the priority of callbacks, and whether they Handle the input callstack
		const EHeartInputLayer Layer = None;

		friend bool operator<(const FConditionalInputBase& A, const FConditionalInputBase& B)
		{
			// Sort in reverse. Higher layers should be ordered first, lower layers after.
			return A.Layer > B.Layer;
		}
	};

	struct FConditionalCallback : FConditionalInputBase
	{
		// Callback to execute the event
		const TSharedPtr<FDelegateBase> Handler;
	};

	struct FConditionalCallback_DDO : FConditionalInputBase
	{
		// Callback to begin a DDO
		const TSharedPtr<FDelegateBase> Handler;
	};
}