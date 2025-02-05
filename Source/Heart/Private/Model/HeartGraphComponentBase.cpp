// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphComponentBase.h"

void UHeartGraphComponentBase::PostInitProperties()
{
	Super::PostInitProperties();
	if (!Guid.IsValid())
	{
		Guid = FHeartExtensionGuid::New();
	}
}