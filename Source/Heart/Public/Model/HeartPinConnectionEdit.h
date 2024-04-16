// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphPinReference.h"
#include "HeartGuids.h"

class UHeartGraph;
class UHeartGraphNode;

namespace Heart::Connections
{
	// Container for batching pin connection edits.
	class HEART_API FEdit
	{
	public:
		class HEART_API FMemento
		{
			friend FEdit;
			TMap<FHeartPinGuid, FHeartGraphPinConnections> PinConnections;

			friend FArchive& operator<<(FArchive& Ar, FMemento& V)
			{
				return Ar << V.PinConnections;
			}
		};

		FEdit(UHeartGraph* Graph)
		  : Graph(Graph) {}

		~FEdit();

		FEdit& Connect(const FHeartGraphPinReference& PinA, const FHeartGraphPinReference& PinB);

		FEdit& Disconnect(const FHeartGraphPinReference& PinA, const FHeartGraphPinReference& PinB);

		// Remove all connections from a specific pin
		FEdit& DisconnectAll(const FHeartGraphPinReference& Pin);

		// Remove all connections from every pin on a node
		FEdit& DisconnectAll(const FHeartNodeGuid& NodeGuid);

		// Manually set the connections on for a pin. WARNING: this is a very dangerous function as it does not check
		// for cross-node validity. It is up to the callsite to ensure that both nodes linked get updated.
		FEdit& Override(const FHeartGraphPinReference& Pin, const FHeartGraphPinConnections& Connections);

		// Create a memento for each pin connected to this pin, as well as itself
		FEdit& CreateMementos(const FHeartGraphPinReference& Pin, TMap<FHeartNodeGuid, FMemento>& OutMementos);

		// Create a memento for each pin on this node, as well as all connected pins
		FEdit& CreateAllMementos(const FHeartNodeGuid& NodeGuid, TMap<FHeartNodeGuid, FMemento>& OutMementos);

		FEdit& RestoreMementos(const TMap<FHeartNodeGuid, FMemento>& Mementos);

		[[nodiscard]] bool Modified() const { return !ChangedPins.IsEmpty(); }

	private:
		void Internal_Disconnect(UHeartGraphNode* NodeA, const FHeartGraphPinReference& PinA, UHeartGraphNode* NodeB, const FHeartGraphPinReference& PinB);

		UHeartGraph* Graph;
		TMultiMap<UHeartGraphNode*, FHeartPinGuid> ChangedPins;
	};
}