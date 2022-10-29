// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartNodeRegistrySubsystem.h"
#include "Model/GraphNodeRegistrar.h"
#include "ModelView/HeartGraphNode.h"

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

TSubclassOf<UHeartGraphNode> UHeartGraphNodeRegistry::GetGraphNodeClassForNode(UClass* NodeClass) const
{
	if (auto&& FoundClass = GraphNodeMap.Find(NodeClass))
	{
		return *FoundClass;
	}

	if (const auto SuperClass = NodeClass->GetSuperClass())
	{
		return GetGraphNodeClassForNode(SuperClass);
	}

	return nullptr;
}

void UHeartGraphNodeRegistry::AddRegistrar(UGraphNodeRegistrar* Registrar)
{
	NodeClasses.Append(Registrar->NodeClasses);

	for (auto GraphNodeClass : Registrar->GraphNodeClasses)
	{
		if (UClass* SupportedClass = GetDefault<UHeartGraphNode>(GraphNodeClass)->GetSupportedClass())
		{
			GraphNodeMap.Add(SupportedClass, GraphNodeClass);
		}
	}
}

void UHeartGraphNodeRegistry::RemoveRegistrar(UGraphNodeRegistrar* Registrar)
{

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
	for (auto BehaviorClass : Registrar->RegisterWith)
	{
		GetRegistry(BehaviorClass)->AddRegistrar(Registrar);
	}
}

void UHeartNodeRegistrySubsystem::RemoveRegistrar(UGraphNodeRegistrar* Registrar)
{
	for (auto BehaviorClass : Registrar->RegisterWith)
	{
		GetRegistry(BehaviorClass)->RemoveRegistrar(Registrar);
	}
}
