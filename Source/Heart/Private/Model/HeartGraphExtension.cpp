// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphExtension.h"
#include "Model/HeartGraph.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphExtension)

UHeartGraph* UHeartGraphExtension::GetGraph() const
{
	return GetTypedOuter<UHeartGraph>();
}