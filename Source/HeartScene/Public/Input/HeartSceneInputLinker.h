// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartInputLinkerBase.h"
#include "HeartSceneInputLinker.generated.h"

/**
 *
 */
UCLASS()
class HEARTSCENE_API UHeartSceneInputLinker : public UHeartInputLinkerBase
{
	GENERATED_BODY()

public:
	virtual bool InputKey(USceneComponent* Target, const FInputKeyParams& Params);
};


namespace Heart::Input
{
	template <>
	struct TLinkerType<USceneComponent>
	{
		static constexpr bool Supported = true;

		using FValueType = USceneComponent*;

		template <typename T>
		static FORCEINLINE T DefaultReply()
		{
			if constexpr (std::is_same_v<T, void>)
			{
				return;
			}
			else return {};
		}

		HEARTSCENE_API static UHeartSceneInputLinker* FindLinker(const USceneComponent* Component);
	};
}