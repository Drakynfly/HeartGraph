#pragma once

#include "StructView.h"

/**
 * @todo
 * This is a temporary measure to have a templated FStructView. UE will ship with this by default eventually:
 * https://github.com/EpicGames/UnrealEngine/commit/f8e260c8e473fd18de41ae092c6211352812e3a0
 * Probably not until 5.5 tho.
 */
template<typename BaseStructT>
struct TStructView : FStructView
{
public:
	explicit TStructView() = default;

	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	TStructView(BaseStructT& InStruct)
		: FStructView(TBaseStructure<BaseStructT>::Get(), reinterpret_cast<uint8*>(&InStruct))
	{}

	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	explicit TStructView(uint8* InStructMemory = nullptr)
		: FStructView(T::StaticStruct(), InStructMemory)
	{}

	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	TStructView(TInstancedStruct<T>& InstancedStruct)
		: TStructView(InstancedStruct.GetScriptStruct(), InstancedStruct.GetMutableMemory())
	{}

	/*
	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	TStructView(const TSharedStruct<T>& SharedStruct)
		: TStructView(SharedStruct.GetScriptStruct(), SharedStruct.GetMemory())
	{}
	*/

	/** Returns mutable reference to the struct, this getter assumes that all data is valid. */
	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	T& Get() const
	{
		return UE::StructUtils::GetStructRef<BaseStructT>(ScriptStruct, StructMemory);
	}

	/** Returns mutable pointer to the struct, or nullptr if cast is not valid. */
	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	T* GetPtr() const
	{
		return UE::StructUtils::GetStructPtr<T>(ScriptStruct, StructMemory);
	}

	/** Comparison operators. Note: it does not compare the internal structure itself */
	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	bool operator==(const TStructView<T>& Other) const
	{
		return ((ScriptStruct == Other.GetScriptStruct()) && (StructMemory == Other.GetMemory()));
	}

	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	bool operator!=(const TStructView<T>& Other) const
	{
		return !operator==(Other);
	}
};

template<typename BaseStructT>
struct TConstStructView : FConstStructView
{
	explicit TConstStructView() = default;

	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	TConstStructView(const BaseStructT& InStruct)
		: FConstStructView(TBaseStructure<T>::Get(), reinterpret_cast<const uint8*>(&InStruct))
	{}

	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	explicit TConstStructView(const uint8* InStructMemory = nullptr)
		: FConstStructView(T::StaticStruct(), InStructMemory)
	{}

	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	TConstStructView(const TInstancedStruct<T>& InstancedStruct)
		: FConstStructView(InstancedStruct.GetScriptStruct(), InstancedStruct.GetMemory())
	{}

	/*
	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	TConstStructView(const TSharedStruct<T>& SharedStruct)
		: FConstStructView(SharedStruct.GetScriptStruct(), SharedStruct.GetMemory())
	{}

	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	TConstStructView(const TConstSharedStruct<T>& SharedStruct)
		: FConstStructView(SharedStruct.GetScriptStruct(), SharedStruct.GetMemory())
	{}
	*/

	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	TConstStructView(const TStructView<T> StructView)
		: FConstStructView(StructView.GetScriptStruct(), StructView.GetMemory())
	{}

	/** Returns const reference to the struct, this getter assumes that all data is valid. */
	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	const T& Get() const
	{
		return UE::StructUtils::GetStructRef<T>(ScriptStruct, StructMemory);
	}

	/** Returns const pointer to the struct, or nullptr if cast is not valid. */
	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	const T* GetPtr() const
	{
		return UE::StructUtils::GetStructPtr<T>(ScriptStruct, StructMemory);
	}

	const BaseStructT* operator->() const
	{
		return GetPtr();
	}

	/** Comparison operators. Note: it does not compare the internal structure itself */
	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	bool operator==(const TConstStructView<T>& Other) const
	{
		return ((ScriptStruct == Other.GetScriptStruct()) && (StructMemory == Other.GetMemory()));
	}

	template<typename T = BaseStructT, typename = std::enable_if_t<std::is_base_of_v<BaseStructT, std::decay_t<T>>>>
	bool operator!=(const TConstStructView<T>& Other) const
	{
		return !operator==(Other);
	}
};