// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphCanvasConnection.h"
#include "UMG/HeartGraphCanvas.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphCanvasConnection)

UHeartInputLinkerBase* UHeartGraphCanvasConnection::ResolveLinker_Implementation() const
{
	return Execute_ResolveLinker(GraphCanvas.Get());
}