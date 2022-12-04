// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartGraphSettings.h"
#include "GraphRegistry/GraphNodeRegistrar.h"
#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "GraphRegistry/HeartNodeRegistrySubsystem.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPin.h"
#include "View/HeartVisualizerInterfaces.h"

void UHeartGraphNodeRegistry::NotifyNodeBlueprintNodeClassAdded(TSubclassOf<UHeartGraphNode> GraphNodeClass)
{
	if (IsValid(GraphNodeClass))
	{
		for (auto&& Registrar : ContainedRegistrars)
		{
			if (Registrar && Registrar->Recursive)
			{

			}
		}
	}
}

void UHeartGraphNodeRegistry::AddRegistrationList(const FHeartRegistrationClasses& Registration)
{
	for (auto&& NodeClass : Registration.NodeClasses)
	{
		++NodeClasses.FindOrAdd(NodeClass.Get());
	}

	for (auto&& GraphNodeClass : Registration.GraphNodeClasses)
	{
		if (auto&& ClassObj = GraphNodeClass)
		{
			if (UClass* SupportedClass = GetDefault<UHeartGraphNode>(ClassObj)->GetSupportedClass())
			{
				GraphNodeMap.Add(SupportedClass, ClassObj);
			}
		}
	}

	for (auto&& NodeVisualizerClass : Registration.NodeVisualizerClasses)
	{
		if (auto&& ClassObj = NodeVisualizerClass)
		{
			if (UClass* SupportedClass =
					IGraphNodeVisualizerInterface::Execute_GetSupportedGraphNodeClass(ClassObj->GetDefaultObject()))
			{
				if (auto&& Ref = NodeVisualizerMap.Find(SupportedClass))
				{
					Ref++;
				}
				else
				{
					NodeVisualizerMap.Add(SupportedClass, ClassObj.Get());
				}
			}
		}
	}

	for (auto&& PinVisualizerClass : Registration.PinVisualizerClasses)
	{
		if (auto&& ClassObj = PinVisualizerClass)
		{
			if (UClass* SupportedClass =
				   IGraphPinVisualizerInterface::Execute_GetSupportedGraphPinClass(ClassObj->GetDefaultObject()))
			{
				if (auto&& Ref = PinVisualizerMap.Find(SupportedClass))
				{
					Ref++;
				}
				else
				{
					PinVisualizerMap.Add(SupportedClass, ClassObj.Get());
				}
			}
		}
	}
}

void UHeartGraphNodeRegistry::RemoveRegistrationList(const FHeartRegistrationClasses& Registration)
{
	for (auto&& NodeClass : Registration.NodeClasses)
	{
		if (auto* ClassRef = NodeClasses.Find(NodeClass.Get()))
		{
			--*ClassRef;
			if (ClassRef->GetRefCount() == 0)
			{
				NodeClasses.Remove(*ClassRef);
			}
		}
	}

	for (auto&& GraphNodeClass : Registration.GraphNodeClasses)
	{
		if (auto&& ClassObj = GraphNodeClass.Get())
		{
			if (UClass* SupportedClass = GetDefault<UHeartGraphNode>(ClassObj)->GetSupportedClass())
			{
				GraphNodeMap.Remove(SupportedClass);
			}
		}
	}

	for (auto&& NodeVisualizerClass : Registration.NodeVisualizerClasses)
	{
		if (UClass* SupportedClass =
			IGraphNodeVisualizerInterface::Execute_GetSupportedGraphNodeClass(NodeVisualizerClass->GetDefaultObject()))
		{
			NodeVisualizerMap.Remove(SupportedClass);
		}
	}

	for (auto&& PinVisualizerClass : Registration.PinVisualizerClasses)
	{
		if (UClass* SupportedClass =
			IGraphPinVisualizerInterface::Execute_GetSupportedGraphPinClass(PinVisualizerClass->GetDefaultObject()))
		{
			PinVisualizerMap.Remove(SupportedClass);
		}
	}
}

TArray<FString> UHeartGraphNodeRegistry::GetNodeCategories() const
{
	TSet<FString> UnsortedCategories;
	UnsortedCategories.Reserve(GraphNodeMap.Num());

	for (auto&& HeartGraphNodeClass : GraphNodeMap)
	{
		if (auto&& DefaultObject = HeartGraphNodeClass.Value->GetDefaultObject<UHeartGraphNode>())
		{
			UnsortedCategories.Emplace(DefaultObject->GetNodeCategory().ToString());
		}
	}

	TArray<FString> SortedCategories = UnsortedCategories.Array();
	SortedCategories.Sort();
	return SortedCategories;
}

void UHeartGraphNodeRegistry::GetNodeClasses(TArray<UClass*>& OutClasses) const
{
	for (auto&& NodeClass : NodeClasses)
	{
		OutClasses.Add(NodeClass.Class);
	}
}

void UHeartGraphNodeRegistry::GetFilteredNodeClasses(const FNativeNodeClassFilter& Filter,
                                                     TArray<UClass*>& OutClasses) const
{
	if (!ensure(Filter.IsBound()))
	{
		return;
	}

	for (auto&& NodeClass : NodeClasses)
	{
		if (ensure(IsValid(NodeClass.Class)))
		{
			if (Filter.Execute(NodeClass.Class))
			{
				OutClasses.Add(NodeClass.Class);
			}
		}
	}
}

void UHeartGraphNodeRegistry::GetFilteredNodeClassesWithGraphClass(const FNativeNodeClassFilter& Filter,
	TMap<UClass*, TSubclassOf<UHeartGraphNode>>& OutClasses) const
{
	if (!ensure(Filter.IsBound()))
	{
		return;
	}

	for (auto&& NodeClass : NodeClasses)
	{
		if (ensure(IsValid(NodeClass.Class)))
		{
			if (Filter.Execute(NodeClass.Class))
			{
				auto&& GraphNodeClass = GetGraphNodeClassForNode(NodeClass.Class);
				OutClasses.Add(NodeClass.Class, GraphNodeClass);
			}
		}
	}
}

void UHeartGraphNodeRegistry::GetFilteredNodeClasses(const FNodeClassFilter& Filter, TArray<UClass*>& OutClasses) const
{
	if (!ensure(Filter.IsBound()))
	{
		return;
	}

	for (auto&& NodeClass : NodeClasses)
	{
		if (ensure(IsValid(NodeClass.Class)))
		{
			if (Filter.Execute(NodeClass.Class))
			{
				OutClasses.Add(NodeClass.Class);
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
		if (auto&& FoundRef = NodeVisualizerMap.Find(Class))
		{
			return FoundRef->Class;
		}
	}

	// Try and retrieve a fallback visualizer
	// @todo this might return a widget class in cases where that is not expected. maybe allow a filter on this function
	if (auto&& Subsystem = GEngine->GetEngineSubsystem<UHeartNodeRegistrySubsystem>())
	{
		if (auto&& Fallback = Subsystem->GetFallbackRegistrar())
		{
			if (ensure(Fallback->Registration.NodeVisualizerClasses.IsValidIndex(0)))
			{
				return Fallback->Registration.NodeVisualizerClasses[0];
			}
		}
	}

	UE_LOG(LogHeartNodeRegistry, Warning, TEXT("Registry was unable to find a visualizer for class: %s"), *GraphNodeClass->GetName())

	return nullptr;
}

UClass* UHeartGraphNodeRegistry::GetVisualizerClassForGraphPin(const TSubclassOf<UHeartGraphPin> GraphPinClass) const
{
	for (UClass* Class = GraphPinClass; Class; Class = Class->GetSuperClass())
	{
		if (auto&& FoundRef = PinVisualizerMap.Find(Class))
		{
			return FoundRef->Class;
		}
	}

	// Try and retrieve a fallback visualizer
	// @todo this might return a widget class in cases where that is not expected. maybe allow a filter on this function
	if (auto&& Subsystem = GEngine->GetEngineSubsystem<UHeartNodeRegistrySubsystem>())
	{
		if (auto&& Fallback = Subsystem->GetFallbackRegistrar())
		{
			if (ensure(Fallback->Registration.PinVisualizerClasses.IsValidIndex(0)))
			{
				return Fallback->Registration.PinVisualizerClasses[0];
			}
		}
	}

	UE_LOG(LogHeartNodeRegistry, Warning, TEXT("Registry was unable to find a visualizer for class: %s"), *GraphPinClass->GetName())

	return nullptr;
}

void UHeartGraphNodeRegistry::AddRegistrar(UGraphNodeRegistrar* Registrar)
{
	if (!ensure(IsValid(Registrar)))
	{
		return;
	}

	// Only allow registry once
	if (ContainedRegistrars.Contains(Registrar))
	{
		// We really can't warn against this, since the editor tries to re-add everything occasionally
		//UE_LOG(LogHeartNodeRegistry, Warning, TEXT("Tried to add Registrar that was already registered!"));
		return;
	}

	AddRegistrationList(Registrar->Registration);

	ContainedRegistrars.Add(Registrar);
}

void UHeartGraphNodeRegistry::RemoveRegistrar(UGraphNodeRegistrar* Registrar)
{
	if (!ensure(IsValid(Registrar)))
	{
		return;
	}

	if (!ContainedRegistrars.Contains(Registrar))
	{
		// We really can't warn against this, since the registrars try to remove themselves precautionarily
		//UE_LOG(LogHeartNodeRegistry, Warning, TEXT("Tried to remove Registrar that wasn't registered!"));
		return;
	}

	RemoveRegistrationList(Registrar->Registration);

	ContainedRegistrars.Remove(Registrar);
}

void UHeartGraphNodeRegistry::DeregisterAll()
{
	NodeClasses.Empty();
	GraphNodeMap.Empty();
	NodeVisualizerMap.Empty();
	PinVisualizerMap.Empty();
	ContainedRegistrars.Empty();
}
