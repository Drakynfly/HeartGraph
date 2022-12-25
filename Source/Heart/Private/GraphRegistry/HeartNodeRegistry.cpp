// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartGraphSettings.h"
#include "GraphRegistry/GraphNodeRegistrar.h"
#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "GraphRegistry/HeartNodeRegistrySubsystem.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPin.h"
#include "View/HeartVisualizerInterfaces.h"

bool UHeartGraphNodeRegistry::FilterClassForRegistration(const TObjectPtr<UClass>& Class) const
{
	return IsValid(Class) ? !(Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated)) : false;
}

void UHeartGraphNodeRegistry::AddRegistrationList(const FHeartRegistrationClasses& Registration)
{
	for (auto&& GraphNodeList : Registration.GraphNodeLists)
	{
		if (GraphNodeList.Value.NodeClasses.IsEmpty())
		{
			continue;
		}

		auto&& CountedList = GraphClasses.FindOrAdd(GraphNodeList.Key);

		for (auto&& NodeClass : GraphNodeList.Value.NodeClasses)
		{
			if (!FilterClassForRegistration(NodeClass))
			{
				continue;
			}

			CountedList.FindOrAdd(NodeClass)++;
		}
	}

	for (auto&& NodeClass : Registration.NodeClasses)
	{
		if (!FilterClassForRegistration(NodeClass))
		{
			continue;
		}
		++NodeClasses.FindOrAdd(NodeClass.Get());
	}

	for (auto&& GraphNodeClass : Registration.GraphNodeClasses)
	{
		if (!FilterClassForRegistration(GraphNodeClass))
		{
			continue;
		}

		if (UClass* SupportedClass = GetDefault<UHeartGraphNode>(GraphNodeClass)->GetSupportedClass())
		{
			GraphNodeMap.Add(SupportedClass, GraphNodeClass);
		}
	}

	for (auto&& NodeVisualizerClass : Registration.NodeVisualizerClasses)
	{
		if (!FilterClassForRegistration(NodeVisualizerClass))
		{
			continue;
		}

		if (UClass* SupportedClass =
				IGraphNodeVisualizerInterface::Execute_GetSupportedGraphNodeClass(NodeVisualizerClass->GetDefaultObject()))
		{
			if (auto&& Ref = NodeVisualizerMap.Find(SupportedClass))
			{
				Ref++;
			}
			else
			{
				NodeVisualizerMap.Add(SupportedClass, NodeVisualizerClass.Get());
			}
		}
	}

	for (auto&& PinVisualizerClass : Registration.PinVisualizerClasses)
	{
		if (!FilterClassForRegistration(PinVisualizerClass))
		{
			continue;
		}

		if (UClass* SupportedClass =
			   IGraphPinVisualizerInterface::Execute_GetSupportedGraphPinClass(PinVisualizerClass->GetDefaultObject()))
		{
			if (auto&& Ref = PinVisualizerMap.Find(SupportedClass))
			{
				Ref++;
			}
			else
			{
				PinVisualizerMap.Add(SupportedClass, PinVisualizerClass.Get());
			}
		}
	}
}

void UHeartGraphNodeRegistry::RemoveRegistrationList(const FHeartRegistrationClasses& Registration)
{
	for (auto&& GraphNodeList : Registration.GraphNodeLists)
	{
		auto* CountedList = GraphClasses.Find(GraphNodeList.Key);
		if (CountedList == nullptr) continue;

		for (auto&& NodeClass : GraphNodeList.Value.NodeClasses)
		{
			int32* ClassCount = CountedList->Find(NodeClass);
			if (ClassCount == nullptr) continue;

			(*ClassCount)--;
			if (*ClassCount <= 0)
			{
				CountedList->Remove(NodeClass);
				if (CountedList->IsEmpty())
				{
					GraphClasses.Remove(GraphNodeList.Key);
				}
			}
		}
	}

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
		if (!IsValid(GraphNodeClass))
		{
			continue;
		}

		if (UClass* SupportedClass = GetDefault<UHeartGraphNode>(GraphNodeClass)->GetSupportedClass())
		{
			GraphNodeMap.Remove(SupportedClass);
		}
	}

	for (auto&& NodeVisualizerClass : Registration.NodeVisualizerClasses)
	{
		if (!IsValid(NodeVisualizerClass))
		{
			continue;
		}

		if (UClass* SupportedClass =
			IGraphNodeVisualizerInterface::Execute_GetSupportedGraphNodeClass(NodeVisualizerClass->GetDefaultObject()))
		{
			NodeVisualizerMap.Remove(SupportedClass);
		}
	}

	for (auto&& PinVisualizerClass : Registration.PinVisualizerClasses)
	{
		if (!IsValid(PinVisualizerClass))
		{
			continue;
		}

		if (UClass* SupportedClass =
			IGraphPinVisualizerInterface::Execute_GetSupportedGraphPinClass(PinVisualizerClass->GetDefaultObject()))
		{
			PinVisualizerMap.Remove(SupportedClass);
		}
	}
}

void UHeartGraphNodeRegistry::SetRecursivelyDiscoveredClasses(const FHeartRegistrationClasses& Classes)
{
	FHeartRegistrationClasses ClassesToRemove;

	for (auto&& NodeClass : RecursivelyDiscoveredClasses.NodeClasses)
	{
		if (!Classes.NodeClasses.Contains(NodeClass))
		{
			ClassesToRemove.NodeClasses.Add(NodeClass);
		}
	}

	for (auto&& GraphNodeClass : RecursivelyDiscoveredClasses.GraphNodeClasses)
	{
		if (!Classes.GraphNodeClasses.Contains(GraphNodeClass))
		{
			ClassesToRemove.GraphNodeClasses.Add(GraphNodeClass);
		}
	}

	for (auto&& NodeVisualizerClass : RecursivelyDiscoveredClasses.NodeVisualizerClasses)
	{
	    if (!Classes.NodeVisualizerClasses.Contains(NodeVisualizerClass))
	    {
		    ClassesToRemove.NodeVisualizerClasses.Add(NodeVisualizerClass);
	    }
	}

    for (auto&& PinVisualizerClass : RecursivelyDiscoveredClasses.PinVisualizerClasses)
    {
        if (!RecursivelyDiscoveredClasses.PinVisualizerClasses.Contains(PinVisualizerClass))
        {
        	ClassesToRemove.PinVisualizerClasses.Add(PinVisualizerClass);
        }
    }

	RemoveRegistrationList(ClassesToRemove);

	FHeartRegistrationClasses ClassesToAdd;

	for (auto&& NodeClass : Classes.NodeClasses)
	{
		if (!RecursivelyDiscoveredClasses.NodeClasses.Contains(NodeClass))
		{
			ClassesToAdd.NodeClasses.Add(NodeClass);
		}
	}

	for (auto&& GraphNodeClass : Classes.GraphNodeClasses)
	{
		if (!RecursivelyDiscoveredClasses.GraphNodeClasses.Contains(GraphNodeClass))
		{
			ClassesToAdd.GraphNodeClasses.Add(GraphNodeClass);
		}
	}

	for (auto&& NodeVisualizerClass : Classes.NodeVisualizerClasses)
	{
		if (!RecursivelyDiscoveredClasses.NodeVisualizerClasses.Contains(NodeVisualizerClass))
		{
			ClassesToAdd.NodeVisualizerClasses.Add(NodeVisualizerClass);
		}
	}

	for (auto&& PinVisualizerClass : Classes.PinVisualizerClasses)
	{
		if (!Classes.PinVisualizerClasses.Contains(PinVisualizerClass))
		{
			ClassesToAdd.PinVisualizerClasses.Add(PinVisualizerClass);
		}
	}

	AddRegistrationList(ClassesToAdd);
}

FHeartRegistrationClasses UHeartGraphNodeRegistry::GetClassesRegisteredRecursively()
{
	FHeartRegistrationClasses Classes;

	for (auto&& Registrar : ContainedRegistrars)
	{
		if (Registrar->Recursive)
		{
			Classes.NodeClasses.Append(Registrar->Registration.NodeClasses);
			Classes.GraphNodeClasses.Append(Registrar->Registration.GraphNodeClasses);
			Classes.NodeVisualizerClasses.Append(Registrar->Registration.NodeVisualizerClasses);
			Classes.PinVisualizerClasses.Append(Registrar->Registration.PinVisualizerClasses);
		}
	}

	return Classes;
}

TArray<FString> UHeartGraphNodeRegistry::GetNodeCategories() const
{
	TSet<FString> UnsortedCategories;

	for (auto&& GraphClassList : GraphClasses)
	{
		if (auto&& DefaultObject = GraphClassList.Key->GetDefaultObject<UHeartGraphNode>())
		{
			UnsortedCategories.Emplace(DefaultObject->GetNodeCategory().ToString());
		}
	}

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
	for (auto&& GraphClassList : GraphClasses)
	{
		// @todo AHH TObjectPtr drives me nuts!!
		TArray<TObjectPtr<UClass>> ClassObjectPtrArray;
		GraphClassList.Value.GetKeys(ClassObjectPtrArray);
		OutClasses.Append(ClassObjectPtrArray);
	}

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

	for (auto&& GraphClassList : GraphClasses)
	{
		for (auto&& CountedClass : GraphClassList.Value)
		{
			UClass* Class = CountedClass.Key;
			if (!ensure(IsValid(Class))) continue;

			if (Filter.Execute(Class))
			{
				OutClasses.Add(Class);
			}
		}
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

	for (auto&& GraphClassList : GraphClasses)
	{
		for (auto&& CountedClass : GraphClassList.Value)
		{
			UClass* Class = CountedClass.Key;
			if (!ensure(IsValid(Class))) continue;

			if (Filter.Execute(Class))
			{
				OutClasses.Add(Class, GraphClassList.Key);
			}
		}
	}

	for (auto&& NodeClass : NodeClasses)
	{
		if (!ensure(IsValid(NodeClass.Class)))
		{
			continue;
		}

		if (Filter.Execute(NodeClass.Class))
		{
			auto&& GraphNodeClass = GetGraphNodeClassForNode(NodeClass.Class);
			OutClasses.Add(NodeClass.Class, GraphNodeClass);
		}
	}
}

void UHeartGraphNodeRegistry::GetFilteredNodeClasses(const FNodeClassFilter& Filter, TArray<UClass*>& OutClasses) const
{
	if (!ensure(Filter.IsBound()))
	{
		return;
	}

	for (auto&& GraphClassList : GraphClasses)
	{
		for (auto&& CountedClass : GraphClassList.Value)
		{
			UClass* Class = CountedClass.Key;
			if (!ensure(IsValid(Class))) continue;

			if (Filter.Execute(Class))
			{
				OutClasses.Add(Class);
			}
		}
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

TSubclassOf<UHeartGraphNode> UHeartGraphNodeRegistry::GetGraphNodeClassForNode(UClass* NodeClass) const
{
	for (UClass* Class = NodeClass; Class; Class = Class->GetSuperClass())
	{
		for (auto&& ClassList : GraphClasses)
		{
			if (ClassList.Value.Contains(NodeClass))
			{
				return ClassList.Key;
			}
		}

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
	GraphClasses.Empty();
	NodeClasses.Empty();
	GraphNodeMap.Empty();
	NodeVisualizerMap.Empty();
	PinVisualizerMap.Empty();
	ContainedRegistrars.Empty();
}
