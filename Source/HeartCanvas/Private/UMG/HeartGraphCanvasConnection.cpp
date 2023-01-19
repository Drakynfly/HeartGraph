// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphCanvasConnection.h"
#include "UMG/HeartGraphCanvas.h"

UHeartWidgetInputLinker* UHeartGraphCanvasConnection::ResolveLinker_Implementation() const
{
	return Execute_ResolveLinker(GraphCanvas.Get());
}
