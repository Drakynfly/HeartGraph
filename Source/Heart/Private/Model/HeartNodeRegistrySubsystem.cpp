// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartNodeRegistrySubsystem.h"
#include "Model/GraphNodeRegistrar.h"
#include "ModelView/HeartGraphNode.h"
#include "ModelView/HeartGraphPin.h"
#include "View/HeartVisualizerInterfaces.h"

void UHeartGraphNodeRegistry::GetFilteredNodeClasses(const FNodeClassFilter& Filter, TArray<UClass*>& OutClasses) const
{
	if (!ensure(Filter.IsBound()))
	{
		return;
	}

	for (auto NodeClass : NodeClasses)
	{
		if (ensure(IsValid(NodeClass)))
		{
			if (Filter.Execute(NodeClass))
			{
				OutClasses.Add(NodeClass);
			}
		}
	}
}

UClass* UHeartGraphNodeRegistry::GetGraphNodeClassForNode(UClass* NodeClass) const
{
	for (UClass* Class = NodeClass; Class; Class = Class->GetSuperClass())
	{
		if (auto&& FoundClass = GraphNodeMap.Find(Class))
		{
			return *FoundClass;
		}
	}

	return nullptr;
}

UClass* UHeartGraphNodeRegistry::GetVisualizerClassForGraphNode(const TSubclassOf<UHeartGraphNode> GraphNodeClass) const
{
	for (UClass* Class = GraphNodeClass; Class; Class = Class->GetSuperClass())
	{
		if (auto&& FoundClass = NodeVisualizerMap.Find(Class))
		{
			return *FoundClass;
		}
	}

	return nullptr;
}

UClass* UHeartGraphNodeRegistry::GetVisualizerClassForGraphPin(const TSubclassOf<UHeartGraphPin> GraphPinClass) const
{
	for (UClass* Class = GraphPinClass; Class; Class = Class->GetSuperClass())
	{
		if (auto&& FoundClass = PinVisualizerMap.Find(Class))
		{
			return *FoundClass;
		}
	}

	return nullptr;
}

void UHeartGraphNodeRegistry::AddRegistrar(UGraphNodeRegistrar* Registrar)
{
	NodeClasses.Append(Registrar->NodeClasses);

	for (auto&& GraphNodeClass : Registrar->GraphNodeClasses)
	{
		if (UClass* SupportedClass = GetDefault<UHeartGraphNode>(GraphNodeClass)->GetSupportedClass())
		{
			GraphNodeMap.Add(SupportedClass, GraphNodeClass);
		}
	}

	for (auto&& NodeVisualizerClass : Registrar->NodeVisualizerClasses)
	{
		if (UClass* SupportedClass =
			IGraphNodeVisualizerInterface::Execute_GetSupportedGraphNodeClass(NodeVisualizerClass->GetDefaultObject()))
		{
			NodeVisualizerMap.Add(SupportedClass, NodeVisualizerClass);
		}
	}

	for (auto&& PinVisualizerClass : Registrar->PinVisualizerClasses)
	{
		if (UClass* SupportedClass =
			IGraphPinVisualizerInterface::Execute_GetSupportedGraphPinClass(PinVisualizerClass->GetDefaultObject()))
		{
			PinVisualizerMap.Add(SupportedClass, PinVisualizerClass);
		}
	}
}

void UHeartGraphNodeRegistry::RemoveRegistrar(UGraphNodeRegistrar* Registrar)
{
	for (auto NodeClass : Registrar->NodeClasses)
	{
		NodeClasses.Remove(NodeClass);
	}

	for (auto&& GraphNodeClass : Registrar->GraphNodeClasses)
	{
		if (UClass* SupportedClass = GetDefault<UHeartGraphNode>(GraphNodeClass)->GetSupportedClass())
		{
			GraphNodeMap.Add(SupportedClass, GraphNodeClass);
		}
	}

	for (auto&& NodeVisualizerClass : Registrar->NodeVisualizerClasses)
	{
		if (UClass* SupportedClass =
			IGraphNodeVisualizerInterface::Execute_GetSupportedGraphNodeClass(NodeVisualizerClass->GetDefaultObject()))
		{
			NodeVisualizerMap.Add(SupportedClass, NodeVisualizerClass);
		}
	}

	for (auto&& PinVisualizerClass : Registrar->PinVisualizerClasses)
	{
		if (UClass* SupportedClass =
			IGraphPinVisualizerInterface::Execute_GetSupportedGraphPinClass(PinVisualizerClass->GetDefaultObject()))
		{
			PinVisualizerMap.Add(SupportedClass, PinVisualizerClass);
		}
	}
}

UHeartGraphNodeRegistry* UHeartNodeRegistrySubsystem::GetRegistry(const TSubclassOf<UHeartGraph> Class)
{
	if (auto&& FoundRegistry = NodeRegistries.Find(Class))
	{
		return *FoundRegistry;
	}

	const auto NewRegistry = NewObject<UHeartGraphNodeRegistry>(this);
	return NodeRegistries.Add(Class, NewRegistry);
}

void UHeartNodeRegistrySubsystem::AddRegistrar(UGraphNodeRegistrar* Registrar)
{
	for (auto GraphClass : Registrar->RegisterWith)
	{
		GetRegistry(GraphClass)->AddRegistrar(Registrar);
	}
}

void UHeartNodeRegistrySubsystem::RemoveRegistrar(UGraphNodeRegistrar* Registrar)
{
	for (auto GraphClass : Registrar->RegisterWith)
	{
		GetRegistry(GraphClass)->RemoveRegistrar(Registrar);
	}
}
