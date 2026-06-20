// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

namespace Heart::Containers
{
	template <typename TClass = UObject>
	struct TCountedWeakPtr
	{
		TCountedWeakPtr()
		  : Obj(nullptr),
			Count(0)
		{
		}

		TCountedWeakPtr(TWeakObjectPtr<TClass> Obj)
		  : Obj(Obj),
			Count(0)
		{
		}

		TCountedWeakPtr(TClass* Obj)
		  : Obj(TWeakObjectPtr<TClass>(Obj)),
			Count(0)
		{
		}

		const TWeakObjectPtr<TClass> Obj;
		uint32 Count;

		friend TCountedWeakPtr& operator++(TCountedWeakPtr& Value)
		{
			++Value.Count;
			return Value;
		}

		friend TCountedWeakPtr& operator--(TCountedWeakPtr& Value)
		{
			checkSlow(Value.Count > 0)
			--Value.Count;
			return Value;
		}

		[[nodiscard]] UE_REWRITE bool UEOpEquals(const TCountedWeakPtr& Other) const
		{
			return Obj == Other.Obj;
		}

		friend [[nodiscard]] UE_REWRITE uint32 GetTypeHash(const TCountedWeakPtr& Value)
		{
			return HashCombineFast(GetTypeHash(Value.Obj), GetTypeHash(Value.Count));
		}
	};

	template <typename TClass = UObject>
	struct TCountedPtr
	{
		TCountedPtr()
		  : Obj(nullptr),
			Count(0)
		{
		}

		TCountedPtr(TClass* Obj)
		  : Obj(Obj),
			Count(0)
		{
		}

		TObjectPtr<TClass> Obj;
		uint32 Count;

		void Inc()
		{
			Count++;
		}

		int32 Dec()
		{
			Count--;
			return Count;
		}

		[[nodiscard]] UE_REWRITE bool UEOpEquals(const TCountedPtr& Other) const
		{
			return Obj == Other.Obj;
		}

		friend [[nodiscard]] UE_REWRITE uint32 GetTypeHash(const TCountedPtr& Value)
		{
			return HashCombineFast(GetTypeHash(Value.Obj), GetTypeHash(Value.Count));
		}
	};

	template <typename TClass = UObject>
	struct TCountedWeakClassPtr
	{
		TCountedWeakClassPtr()
		  : Obj(nullptr),
			Count(0)
		{
		}

		TCountedWeakClassPtr(const TWeakObjectPtr<TClass> Class)
		  : Obj(Class),
			Count(0)
		{
		}

		TCountedWeakClassPtr(UClass* Class)
		  : Obj(Class),
			Count(0)
		{
		}

		TCountedWeakClassPtr(TSubclassOf<TClass> Class)
		  : Obj(TWeakObjectPtr<UClass>(Class.Get())),
			Count(0)
		{
		}

		const TWeakObjectPtr<UClass> Obj;
		uint32 Count;

		friend TCountedWeakClassPtr& operator++(TCountedWeakClassPtr& Value)
		{
			++Value.Count;
			return Value;
		}

		friend TCountedWeakClassPtr& operator--(TCountedWeakClassPtr& Value)
		{
			checkSlow(Value.Count > 0)
			--Value.Count;
			return Value;
		}

		[[nodiscard]] UE_REWRITE bool UEOpEquals(const TCountedWeakClassPtr& Other) const
		{
			return Obj == Other.Obj;
		}

		friend [[nodiscard]] UE_REWRITE uint32 GetTypeHash(const TCountedWeakClassPtr& Value)
		{
			return HashCombineFast(GetTypeHash(Value.Obj), GetTypeHash(Value.Count));
		}
	};
}