// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphExtension.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphExtension)

void UHeartGraphExtension::PostLoad()
{
	Super::PostLoad();
	if (!Guid.IsValid())
	{
		Guid = FHeartExtensionGuid::New();
	}
}

UHeartGraph* UHeartGraphExtension::GetGraph() const
{
	return GetOuterUHeartGraph();
}