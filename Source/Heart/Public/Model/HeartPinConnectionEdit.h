// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphPinReference.h"
#include "HeartGuids.h"

class UHeartGraph;
class UHeartGraphNode;

namespace Heart::API
{
	// Container for batching pin connection edits.
	class HEART_API FPinEdit
	{
	public:
		class HEART_API FMemento
		{
			friend FPinEdit;
			TMap<FHeartPinGuid, FHeartGraphPinConnections> PinConnections;

			friend FArchive& operator<<(FArchive& Ar, FMemento& V)
			{
				return Ar << V.PinConnections;
			}
		};

		FPinEdit(UHeartGraph* Graph)
		  : Graph(Graph) {}

		FPinEdit(UHeartGraphNode* Node);

		~FPinEdit();

		FPinEdit& Connect(const FHeartGraphPinReference& PinA, const FHeartGraphPinReference& PinB);

		FPinEdit& Disconnect(const FHeartGraphPinReference& PinA, const FHeartGraphPinReference& PinB);

		// Remove all connections from a specific pin
		FPinEdit& DisconnectAll(const FHeartGraphPinReference& Pin);

		// Remove all connections from every pin on a node
		FPinEdit& DisconnectAll(const FHeartNodeGuid& NodeGuid);

		// Manually set the connections on for a pin. WARNING: this is a very dangerous function as it does not check
		// for cross-node validity. It is up to the callsite to ensure that both nodes linked get updated.
		FPinEdit& Override(const FHeartGraphPinReference& Pin, const FHeartGraphPinConnections& Connections);

		// Create a memento for each pin connected to this pin, as well as itself
		FPinEdit& CreateMementos(const FHeartGraphPinReference& Pin, TMap<FHeartNodeGuid, FMemento>& OutMementos);

		// Create a memento for each pin on this node, as well as all connected pins
		FPinEdit& CreateAllMementos(const FHeartNodeGuid& NodeGuid, TMap<FHeartNodeGuid, FMemento>& OutMementos);

		FPinEdit& RestoreMementos(const TMap<FHeartNodeGuid, FMemento>& Mementos);

		[[nodiscard]] bool Modified() const { return !ChangedPins.IsEmpty(); }

	private:
		void Internal_Disconnect(UHeartGraphNode* NodeA, const FHeartGraphPinReference& PinA, UHeartGraphNode* NodeB, const FHeartGraphPinReference& PinB);

		UHeartGraph* Graph;
		TMultiMap<UHeartGraphNode*, FHeartPinGuid> ChangedPins;
	};
}

namespace Heart::Connections
{
	using FEdit UE_DEPRECATED(5.5, "Class renamed and moved to Heart::API::FPinEdit") = Heart::API::FPinEdit;
}