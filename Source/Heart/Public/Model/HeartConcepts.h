// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Templates/RemoveReference.h"
#include "Templates/UnrealTypeTraits.h"

class UHeartGraph;
class UHeartGraphNode;
class UHeartGraphExtension;
class UHeartGraphNodeComponent;

namespace Heart
{
	template<typename T>
	concept CGraph = TIsDerivedFrom<typename TRemoveReference<T>::Type, UHeartGraph>::Value;

	template<typename T>
	concept CGraphNode = TIsDerivedFrom<typename TRemoveReference<T>::Type, UHeartGraphNode>::Value;

	template<typename T>
	concept CGraphExtension = TIsDerivedFrom<typename TRemoveReference<T>::Type, UHeartGraphExtension>::Value;

	template<typename T>
	concept CGraphNodeComponent = TIsDerivedFrom<typename TRemoveReference<T>::Type, UHeartGraphNodeComponent>::Value;
}