// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

namespace DoOnceSupport
{
	/**
	 * Not meant for direct consumption: use DO_ONCE instead.
	 *
	 * RAII class that calls a lambda the first time it is created.
	 *
	 * Based on Runtime/Core/Public/Misc/ScopeExit.h
	 */
	template <typename FuncType>
	class TScopeGuard
	{
		TScopeGuard(TScopeGuard&&) = delete;
		TScopeGuard(const TScopeGuard&) = delete;
		TScopeGuard& operator=(TScopeGuard&&) = delete;
		TScopeGuard& operator=(const TScopeGuard&) = delete;

	public:
		// Given a lambda, constructs an RAII scope guard.
		explicit TScopeGuard(FuncType&& InFunc)
		{
			if (!bHasRan)
			{
				InFunc();
			}
			bHasRan = true;
		}

	private:
		static inline bool bHasRan = false;
	};

	struct FScopeGuardSyntaxSupport
	{
		template <typename FuncType>
		TScopeGuard<FuncType> operator+(FuncType&& InFunc)
		{
			return TScopeGuard<FuncType>((FuncType&&)InFunc);
		}
	};
}

#define UE_PRIVATE_DO_ONCE_JOIN(A, B) UE_PRIVATE_DO_ONCE_JOIN_INNER(A, B)
#define UE_PRIVATE_DO_ONCE_JOIN_INNER(A, B) A##B

#define DO_ONCE const auto UE_PRIVATE_DO_ONCE_JOIN(ScopeGuard_, __LINE__) = ::DoOnceSupport::FScopeGuardSyntaxSupport() + [&]()