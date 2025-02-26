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

		friend bool operator==(const TCountedWeakPtr& Lhs, const TCountedWeakPtr& Rhs)
		{
			return Lhs.Obj == Rhs.Obj;
		}

		friend bool operator!=(const TCountedWeakPtr& Lhs, const TCountedWeakPtr& Rhs) { return !(Lhs == Rhs); }

		friend uint32 GetTypeHash(TCountedWeakPtr Value)
		{
			return HashCombine(GetTypeHash(Value.Obj), GetTypeHash(Value.Count));
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

		friend bool operator==(const TCountedPtr& Lhs, const TCountedPtr& Rhs)
		{
			return Lhs.Obj == Rhs.Obj;
		}

		friend bool operator!=(const TCountedPtr& Lhs, const TCountedPtr& Rhs) { return !(Lhs == Rhs); }

		friend uint32 GetTypeHash(TCountedPtr Value)
		{
			return HashCombine(GetTypeHash(Value.Obj), GetTypeHash(Value.Count));
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

		friend bool operator==(const TCountedWeakClassPtr& Lhs, const TCountedWeakClassPtr& Rhs)
		{
			return Lhs.Obj == Rhs.Obj;
		}

		friend bool operator!=(const TCountedWeakClassPtr& Lhs, const TCountedWeakClassPtr& Rhs) { return !(Lhs == Rhs); }

		friend uint32 GetTypeHash(TCountedWeakClassPtr Value)
		{
			return HashCombine(GetTypeHash(Value.Obj), GetTypeHash(Value.Count));
		}
	};
}