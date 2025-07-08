// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphComponentBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphComponentBase)

void UHeartGraphComponentBase::PostInitProperties()
{
	Super::PostInitProperties();
	if (!Guid.IsValid())
	{
		Guid = FHeartExtensionGuid::New();
	}
}