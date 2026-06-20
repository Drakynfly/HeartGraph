// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphComponentBase.h"
#include "Model/HeartGraph.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphComponentBase)

void UHeartGraphComponentBase::PostInitProperties()
{
	Super::PostInitProperties();
	if (!Guid.IsValid())
	{
		Guid = FHeartExtensionGuid::New();
	}
}

UHeartGraph* UHeartGraphComponentBase::GetGraph() const
{
	return GetTypedOuter<UHeartGraph>();
}
