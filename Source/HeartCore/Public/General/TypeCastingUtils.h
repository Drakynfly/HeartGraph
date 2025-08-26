// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

namespace Type
{
	template <typename TOut, typename TIn>
	[[nodiscard]] static TOut Cast(TIn&& In)
	{
		return *reinterpret_cast<TOut*>(&In);
	}
}