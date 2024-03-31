// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

namespace Heart::Graph
{
#if WITH_EDITOR
	// Metadata added to UProperties on children of UHeartGraphNode to trigger reconstruction of pins on the node
	// in the editor.
	static const FLazyName Metadata_TriggersReconstruct("TriggersReconstruct");
#endif
}