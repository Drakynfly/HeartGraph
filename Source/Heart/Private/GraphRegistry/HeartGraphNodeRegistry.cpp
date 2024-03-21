// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "GraphRegistry/GraphNodeRegistrar.h"
#include "GraphRegistry/HeartRegistryQuery.h"
#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#include "Model/HeartGraphNode.h"
#include "View/HeartVisualizerInterfaces.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNodeRegistry)

bool UHeartGraphNodeRegistry::FilterObjectForRegistration(const UObject* Object) const
{
	if (!IsValid(Object))
	{
		return false;
	}

	static constexpr EClassFlags BannedClassFlags = CLASS_Deprecated | CLASS_NewerVersionExists;
	static constexpr EObjectFlags BannedObjectFlags = RF_NewerVersionExists | RF_BeginDestroyed | RF_FinishDestroyed;

	if (const UClass* AsClass = Cast<UClass>(Object))
	{
		if (AsClass->HasAnyClassFlags(BannedClassFlags))
		{
			return false;
		}
	}

	if (Object->HasAnyFlags(BannedObjectFlags))
	{
		return false;
	}

	return true;
}

void UHeartGraphNodeRegistry::AddRegistrationList(const FHeartRegistrationClasses& Registration, const bool Broadcast)
{
	for (auto&& GraphNodeList : Registration.GraphNodeLists)
	{
		if (!FilterObjectForRegistration(GraphNodeList.Key))
		{
			continue;
		}

		for (FHeartRegisteredClass Element : GraphNodeList.Value.Classes)
		{
			if (!FilterObjectForRegistration(Element.Class))
			{
				continue;
			}

			FNodeSourceEntry& Entry = NodeRootTable.FindOrAdd(FHeartNodeSource(Element.Class));

			++Entry.GraphNodes.FindOrAdd(
				Heart::Containers::TCountedWeakClassPtr<UHeartGraphNode>(GraphNodeList.Key));

			// Recursive elements increment a Recursivity counter that gathers derived classes while > 0
			if (Element.Recursive)
			{
				if (Entry.RecursiveRegistryCounter == 0)
				{
					GetDerivedClasses(Element.Class, MutableView(Entry.RecursiveChildren));
				}

				Entry.RecursiveRegistryCounter++;
			}
		}

		for (auto&& Element : GraphNodeList.Value.Objects)
		{
			if (!FilterObjectForRegistration(Element))
			{
				continue;
			}

			++NodeRootTable.FindOrAdd(FHeartNodeSource(Element))
				.GraphNodes.FindOrAdd(Heart::Containers::TCountedWeakClassPtr<UHeartGraphNode>(GraphNodeList.Key));
		}

		for (auto&& NodeVisualizer : GraphNodeList.Value.Visualizers)
		{
			if (!FilterObjectForRegistration(NodeVisualizer))
			{
				continue;
			}

			++NodeVisualizerMap.FindOrAdd(GraphNodeList.Key).FindOrAdd({NodeVisualizer.Get()});
		}
	}

	for (auto Element : Registration.GraphPinLists)
	{
		if (!Element.Key.IsValid() ||
			Element.Key == FHeartGraphPinTag::GetRootTag())
		{
			continue;
		}

		for (auto&& PinVisualizer : Element.Value.PinVisualizers)
		{
			if (!FilterObjectForRegistration(PinVisualizer))
			{
				continue;
			}

			++PinVisualizerMap.FindOrAdd(Element.Key).FindOrAdd({PinVisualizer.Get()});
		}

		for (auto&& ConnectionVisualizerClass : Element.Value.ConnectionVisualizers)
		{
			if (!FilterObjectForRegistration(ConnectionVisualizerClass))
			{
				continue;
			}

			checkSlow(ConnectionVisualizerClass.Key.IsValid())

			++ConnectionVisualizerMap.FindOrAdd(Element.Key).FindOrAdd({ConnectionVisualizerClass.Get()});
		}
	}

	if (Broadcast)
	{
		BroadcastChange();
	}
}

void UHeartGraphNodeRegistry::RemoveRegistrationList(const FHeartRegistrationClasses& Registration, const bool Broadcast)
{
	for (const TTuple<TSubclassOf<UHeartGraphNode>, FHeartNodeClassList>& Element : Registration.GraphNodeLists)
	{
		for (FHeartRegisteredClass Object : Element.Value.Classes)
		{
			FHeartNodeSource SrcClass(Object.Class);

			auto* Entry = NodeRootTable.Find(SrcClass);
			if (Entry == nullptr) continue;

			if (auto&& ObjPtr = Entry->GraphNodes.Find(Element.Key))
			{
				if ((--*ObjPtr).Count == 0)
				{
					Entry->GraphNodes.Remove(Element.Key);
					if (Entry->GraphNodes.IsEmpty())
					{
						NodeRootTable.Remove(SrcClass);
						continue;
					}
				}
			}

			// Recursive Elements decrement their own counter, and remove RecursiveChildren at 0.
			if (Object.Recursive)
			{
				checkSlow(Entry->RecursiveRegistryCounter > 0);
				if (--Entry->RecursiveRegistryCounter == 0)
				{
					Entry->RecursiveChildren.Empty();
				}
			}
		}

		for (auto&& Object : Element.Value.Objects)
		{
			FHeartNodeSource SrcObj(Object);

			auto* Entry = NodeRootTable.Find(SrcObj);
			if (Entry == nullptr) continue;

			if (auto&& ObjPtr = Entry->GraphNodes.Find(Element.Key))
			{
				if ((--*ObjPtr).Count == 0)
				{
					Entry->GraphNodes.Remove(Element.Key);
					if (Entry->GraphNodes.IsEmpty())
					{
						NodeRootTable.Remove(SrcObj);
					}
				}
			}
		}

		for (auto&& NodeVisualizer : Element.Value.Visualizers)
		{
			if (!IsValid(NodeVisualizer))
			{
				continue;
			}

			if (auto&& ObjPtrs = NodeVisualizerMap.Find(Element.Key))
			{
				if (auto&& ObjPtr = ObjPtrs->Find(NodeVisualizer.Get()))
				{
					if ((--*ObjPtr).Count == 0)
					{
						ObjPtrs->Remove(*ObjPtr);
						if (ObjPtrs->IsEmpty())
						{
							NodeVisualizerMap.Remove(Element.Key);
						}
					}
				}
			}
		}
	}

	for (auto Element : Registration.GraphPinLists)
	{
		if (Element.Key.IsValid())
		{
			continue;
		}

		for (auto&& PinVisualizer : Element.Value.PinVisualizers)
		{
			if (!IsValid(PinVisualizer))
			{
				continue;
			}

			if (auto&& ObjPtrs = PinVisualizerMap.Find(Element.Key))
			{
				if (auto&& ObjPtr = ObjPtrs->Find(PinVisualizer.Get()))
				{
					if ((--*ObjPtr).Count == 0)
					{
						ObjPtrs->Remove(*ObjPtr);
						if (ObjPtrs->IsEmpty())
						{
							PinVisualizerMap.Remove(Element.Key);
						}
					}
				}
			}
		}

		for (auto ConnectionVisualizer : Element.Value.ConnectionVisualizers)
		{
			if (!IsValid(ConnectionVisualizer))
			{
				continue;
			}

			if (auto&& ObjPtrs = PinVisualizerMap.Find(Element.Key))
			{
				if (auto&& ObjPtr = ObjPtrs->Find(ConnectionVisualizer.Get()))
				{
					if ((--*ObjPtr).Count == 0)
					{
						ObjPtrs->Remove(*ObjPtr);
						if (ObjPtrs->IsEmpty())
						{
							PinVisualizerMap.Remove(Element.Key);
						}
					}
				}
			}
		}
	}

	if (Broadcast)
	{
		BroadcastChange();
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UHeartGraphNodeRegistry::BroadcastChange()
{
	OnRegistryChangedNative.Broadcast(this);
	{
#if WITH_EDITOR
		FEditorScriptExecutionGuard EditorScriptExecutionGuard;
#endif
		OnRegistryChanged.Broadcast(this);
	}
}

Heart::Query::FRegistryQueryResult UHeartGraphNodeRegistry::QueryRegistry() const
{
	return Heart::Query::FRegistryQueryResult(this);
}

void UHeartGraphNodeRegistry::ForEachNodeObjectClass(const TFunctionRef<bool(const FHeartNodeArchetype&)>& Iter) const
{
	for (auto&& It : Heart::Query::FRegistryQueryResult::FRange(this))
	{
		checkSlow(It.Value.GraphNode.Get());
		checkSlow(It.Value.Source.IsValid());
		if (!Iter({It.Value.GraphNode.Get(), It.Value.Source}))
		{
			return;
		}
	}
}

bool UHeartGraphNodeRegistry::IsRegistered(const UGraphNodeRegistrar* Registrar) const
{
	return ContainedRegistrars.Contains(Registrar);
}

int32 UHeartGraphNodeRegistry::GetNumNodes(const bool IncludeRecursive) const
{
	if (IncludeRecursive)
	{
		int32 OutNum = 0;

		for (const TTuple<FHeartNodeSource, FNodeSourceEntry>& Element : NodeRootTable)
		{
			OutNum++;
			OutNum += Element.Value.RecursiveChildren.Num();
		}
		return OutNum;
	}

	return NodeRootTable.Num();
}

TArray<FString> UHeartGraphNodeRegistry::GetNodeCategories() const
{
	TSet<FString> UnsortedCategories;

	ForEachNodeObjectClass(
		[&UnsortedCategories](const FHeartNodeArchetype& Archetype)
		{
			const UHeartGraphNode* GraphNodeCDO = Archetype.GraphNode->GetDefaultObject<UHeartGraphNode>();
			const UObject* NodeSourceCDO = Archetype.Source.GetDefaultObject();

			if (const FString NodeCategory = GraphNodeCDO->GetNodeCategory(NodeSourceCDO).ToString();
				!NodeCategory.IsEmpty())
			{
				UnsortedCategories.Emplace(NodeCategory);
			}

			return true;
		});

	TArray<FString> SortedCategories = UnsortedCategories.Array();
	SortedCategories.Sort();
	return SortedCategories;
}

void UHeartGraphNodeRegistry::GetAllNodeSources(TArray<FHeartNodeSource>& OutNodeSources) const
{
	OutNodeSources.Empty(GetNumNodes(true));
	for (const TTuple<FHeartNodeSource, FNodeSourceEntry>& Element : NodeRootTable)
	{
		OutNodeSources.Add(Element.Key);
		OutNodeSources.Append(Element.Value.RecursiveChildren);
	}
}

void UHeartGraphNodeRegistry::GetAllGraphNodeArchetypes(TArray<FHeartNodeArchetype>& OutArchetypes) const
{
	OutArchetypes.Empty(GetNumNodes(true));
	for (const TTuple<FHeartNodeSource, FNodeSourceEntry>& Element : NodeRootTable)
	{
		for (auto&& GraphNodePtr : Element.Value.GraphNodes)
		{
			auto&& GraphNode = GraphNodePtr.Obj.Get();
			if (!IsValid(GraphNode))
			{
				continue;
			}

			OutArchetypes.Emplace(GraphNode, Element.Key);
			for (TObjectPtr<UClass> Child : Element.Value.RecursiveChildren)
			{
				OutArchetypes.Emplace(GraphNode, FHeartNodeSource(Child));
			}
		}
	}
}

TSubclassOf<UHeartGraphNode> UHeartGraphNodeRegistry::GetGraphNodeClassForNode(const FHeartNodeSource NodeSource) const
{
	// Cursed for-loop, but it works :)
	for (FHeartNodeSource Test = NodeSource;
		Test.IsValid() && Test != FHeartNodeSource(UObject::StaticClass());
		Test = FHeartNodeSource(Test.NextClass()))
	{
		if (const FNodeSourceEntry* Entry = NodeRootTable.Find(Test))
		{
			return Entry->GraphNodes.Get(FSetElementId::FromInteger(0)).Obj.Get();
		}
	}

	return nullptr;
}

TArray<TSubclassOf<UHeartGraphNode>> UHeartGraphNodeRegistry::GetGraphNodeClassesForNode(
	const FHeartNodeSource NodeSource) const
{
	// Cursed for-loop, but it works :)
	for (FHeartNodeSource Test = NodeSource;
		Test.IsValid() && Test != FHeartNodeSource(UObject::StaticClass());
		Test = FHeartNodeSource(Test.NextClass()))
	{
		if (const FNodeSourceEntry* Entry = NodeRootTable.Find(Test))
		{
			TArray<TSubclassOf<UHeartGraphNode>> GraphNodes;
			for (auto GraphNode : Entry->GraphNodes)
			{
				if (GraphNode.Obj.IsValid())
				{
					GraphNodes.Add(GraphNode.Obj.Get());
				}
			}
			return GraphNodes;
		}
	}

	return {};
}

UClass* UHeartGraphNodeRegistry::GetVisualizerClassForGraphNode(const TSubclassOf<UHeartGraphNode> GraphNodeClass, UClass* VisualizerBase) const
{
	if (!NodeVisualizerMap.IsEmpty())
	{
		for (UClass* Class = GraphNodeClass; Class && Class != UObject::StaticClass(); Class = Class->GetSuperClass())
		{
			if (auto&& ClassMap = NodeVisualizerMap.Find(Class))
			{
				for (auto&& CountedClass : *ClassMap)
				{
					if (!CountedClass.Obj.IsValid())
					{
						continue;
					}

					if (!IsValid(VisualizerBase) ||
						CountedClass.Obj->IsChildOf(VisualizerBase))
					{
						return CountedClass.Obj.Get();
					}
				}
			}
		}
	}

	// Try and retrieve a fallback visualizer
	if (auto&& Subsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		if (auto&& Fallback = Subsystem->GetFallbackRegistrar())
		{
			for (auto&& GraphNodeList : Fallback->GetRegistrationList().GraphNodeLists)
			{
				for (auto Element : GraphNodeList.Value.Visualizers)
				{
					ensure(IsValid(Element));

					if (!IsValid(VisualizerBase) ||
						Element->IsChildOf(VisualizerBase))
					{
						return Element;
					}
				}
			}
		}
	}

	UE_LOG(LogHeartNodeRegistry, Warning, TEXT("Registry was unable to find a node visualizer for class '%s'"), *GraphNodeClass->GetName())

	return nullptr;
}

UClass* UHeartGraphNodeRegistry::GetVisualizerClassForGraphPin(const FHeartGraphPinDesc& GraphPinDesc, UClass* VisualizerBase) const
{
	if (!PinVisualizerMap.IsEmpty())
	{
		for (FHeartGraphPinTag Tag = GraphPinDesc.Tag; Tag.IsValid() && Tag != FHeartGraphPinTag::GetRootTag();
				Tag = FHeartGraphPinTag::TryConvert(Tag.RequestDirectParent()))
		{
			if (auto&& ClassMap = PinVisualizerMap.Find(Tag))
			{
				for (auto&& CountedClass : *ClassMap)
				{
					if (!CountedClass.Obj.IsValid())
					{
						continue;
					}

					if (!IsValid(VisualizerBase) ||
						CountedClass.Obj->IsChildOf(VisualizerBase))
					{
						return CountedClass.Obj.Get();
					}
				}
			}
		}
	}

	// Try and retrieve a fallback visualizer
	if (auto&& Subsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		if (auto&& Fallback = Subsystem->GetFallbackRegistrar())
		{
			for (auto&& GraphPinList : Fallback->GetRegistrationList().GraphPinLists)
			{
				for (auto FallbackClass : GraphPinList.Value.PinVisualizers)
				{
					ensure(IsValid(FallbackClass));

					if (!IsValid(VisualizerBase) ||
						FallbackClass->IsChildOf(VisualizerBase))
					{
						return FallbackClass;
					}
				}
			}
		}
	}

	UE_LOG(LogHeartNodeRegistry, Warning, TEXT("Registry was unable to find a pin visualizer for Tag '%s'"), *GraphPinDesc.Tag.GetTagName().ToString())

	return nullptr;
}

UClass* UHeartGraphNodeRegistry::GetVisualizerClassForGraphConnection(const FHeartGraphPinDesc& FromPinDesc,
																	  const FHeartGraphPinDesc& ToPinDesc,
																	  UClass* VisualizerBase) const
{
	FHeartGraphPinTag SearchTag;

	switch ((FromPinDesc.Tag.IsValid() ? 1 : 0) + (ToPinDesc.Tag.IsValid() ? 2 : 0))
	{
	case 0:
		UE_LOG(LogHeartNodeRegistry, Error, TEXT("Tried to retrieve connection visualizer with two invalid tags!"));
		return nullptr;
	case 1:
		SearchTag = FromPinDesc.Tag;
		break;
	case 2:
		SearchTag = ToPinDesc.Tag;
		break;
	case 3:
		if (FromPinDesc.Tag != ToPinDesc.Tag)
		{
			UE_LOG(LogHeartNodeRegistry, Warning, TEXT("Retrieving connection visualizer with mismatching tags! Defaulting to From Pin Tag"));
		}
		SearchTag = FromPinDesc.Tag;
		break;
	default: ;
	}

	if (!ConnectionVisualizerMap.IsEmpty())
	{
		for (; SearchTag.IsValid() && SearchTag != FHeartGraphPinTag::GetRootTag();
				SearchTag = FHeartGraphPinTag::TryConvert(SearchTag.RequestDirectParent()))
		{
			if (auto&& ClassMap = ConnectionVisualizerMap.Find(SearchTag))
			{
				for (auto&& CountedClass : *ClassMap)
				{
					if (!CountedClass.Obj.IsValid())
					{
						continue;
					}

					if (!IsValid(VisualizerBase) ||
						CountedClass.Obj->IsChildOf(VisualizerBase))
					{
						return CountedClass.Obj.Get();
					}
				}
			}
		}
	}

	// Try and retrieve a fallback visualizer
	if (auto&& Subsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		if (auto&& Fallback = Subsystem->GetFallbackRegistrar())
		{
			for (auto&& GraphPinList : Fallback->GetRegistrationList().GraphPinLists)
			{
				for (auto FallbackClass : GraphPinList.Value.ConnectionVisualizers)
				{
					ensure(IsValid(FallbackClass));

					if (!IsValid(VisualizerBase) ||
						FallbackClass->IsChildOf(VisualizerBase))
					{
						return FallbackClass;
					}
				}
			}
		}
	}

	UE_LOG(LogHeartNodeRegistry, Warning, TEXT("Registry was unable to find a connection visualizer from Tag '%s' to Tag '%s'"),
											 *FromPinDesc.Tag.GetTagName().ToString(), *ToPinDesc.Tag.GetTagName().ToString())

	return nullptr;
}

void UHeartGraphNodeRegistry::AddRegistrar(const UGraphNodeRegistrar* Registrar)
{
	if (!ensure(IsValid(Registrar)))
	{
		return;
	}

	// Only allow registry once
	if (ContainedRegistrars.Contains(Registrar))
	{
		UE_LOG(LogHeartNodeRegistry, Warning, TEXT("Tried to add Registrar that was already registered: '%s'!"), *Registrar->GetName());
		return;
	}

	UE_LOG(LogHeartNodeRegistry, Log, TEXT("HeartGraphNodeRegistry adding registrar '%s'"), *Registrar->GetName())

	Registrar->OnRegistered(this);

	ContainedRegistrars.Add(Registrar);
}

void UHeartGraphNodeRegistry::RemoveRegistrar(const UGraphNodeRegistrar* Registrar)
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

	UE_LOG(LogHeartNodeRegistry, Log, TEXT("HeartGraphNodeRegistry removing registrar '%s'"), *Registrar->GetName())

	Registrar->OnDeregistered(this);

	ContainedRegistrars.Remove(Registrar);
}