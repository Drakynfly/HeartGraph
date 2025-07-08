// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "BloodValue.h"
#include "UObject/Field.h"
#include "Containers/Map.h"

namespace Blood
{
	using FFPropertyReadFunc = TFunctionRef<FBloodValue(const FProperty* ValueProp, const uint8* ValuePtr)>;

	class FPrecomputedMaps
	{
		FPrecomputedMaps();

	public:
		static const FPrecomputedMaps& Get();

		TMap<FFieldClass*, FFPropertyReadFunc> ReaderMap;
	};
}
