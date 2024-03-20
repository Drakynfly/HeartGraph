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
		if (GraphNodeList.Value.Classes.IsEmpty() ||
			!FilterObjectForRegistration(GraphNodeList.Key))
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
	}

	for (auto&& NodeObjectList : Registration.IndividualObjects)
	{
		if (NodeObjectList.Value.Objects.IsEmpty() ||
			!FilterObjectForRegistration(NodeObjectList.Key))
		{
			continue;
		}

		for (auto&& NodeObject : NodeObjectList.Value.Objects)
		{
			if (!FilterObjectForRegistration(NodeObject))
			{
				continue;
			}

			++NodeRootTable.FindOrAdd(FHeartNodeSource(NodeObject))
				.GraphNodes.FindOrAdd(Heart::Containers::TCountedWeakClassPtr<UHeartGraphNode>(NodeObjectList.Key));
		}
	}

	for (auto&& NodeVisualizerClass : Registration.NodeVisualizerClasses)
	{
		if (!FilterObjectForRegistration(NodeVisualizerClass))
		{
			continue;
		}

		if (UClass* SupportedClass =
				IGraphNodeVisualizerInterface::Execute_GetSupportedGraphNodeClass(NodeVisualizerClass->GetDefaultObject());
			IsValid(SupportedClass))
		{
			++NodeVisualizerMap.FindOrAdd(SupportedClass).FindOrAdd({NodeVisualizerClass.Get()});
		}
	}

	for (auto&& PinVisualizerClass : Registration.PinVisualizerClasses)
	{
		if (!FilterObjectForRegistration(PinVisualizerClass))
		{
			continue;
		}

		if (FHeartGraphPinTag SupportedTag =
				IGraphPinVisualizerInterface::Execute_GetSupportedGraphPinTag(PinVisualizerClass->GetDefaultObject());
			SupportedTag.IsValid())
		{
			++PinVisualizerMap.FindOrAdd(SupportedTag).FindOrAdd({PinVisualizerClass.Get()});
		}
	}

	if (Broadcast)
	{
		BroadcastChange();
	}
}

void UHeartGraphNodeRegistry::RemoveRegistrationList(const FHeartRegistrationClasses& Registration, const bool Broadcast)
{
	for (const TTuple<TSubclassOf<UHeartGraphNode>, FClassList>& Element : Registration.GraphNodeLists)
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
	}

	for (const TTuple<TSubclassOf<UHeartGraphNode>, FHeartObjectList>& Element : Registration.IndividualObjects)
	{
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
	}

	for (auto&& NodeVisualizerClass : Registration.NodeVisualizerClasses)
	{
		if (!IsValid(NodeVisualizerClass))
		{
			continue;
		}

		if (TSubclassOf<UHeartGraphNode> SupportedClass =
				IGraphNodeVisualizerInterface::Execute_GetSupportedGraphNodeClass(NodeVisualizerClass->GetDefaultObject());
			IsValid(SupportedClass))
		{
			if (auto&& ObjPtrs = NodeVisualizerMap.Find(SupportedClass))
			{
				if (auto&& ObjPtr = ObjPtrs->Find(NodeVisualizerClass.Get()))
				{
					if ((--*ObjPtr).Count == 0)
					{
						ObjPtrs->Remove(*ObjPtr);
						if (ObjPtrs->IsEmpty())
						{
							NodeVisualizerMap.Remove(SupportedClass);
						}
					}
				}
			}
		}
	}

	for (auto&& PinVisualizerClass : Registration.PinVisualizerClasses)
	{
		if (!IsValid(PinVisualizerClass))
		{
			continue;
		}

		if (const FHeartGraphPinTag SupportedTag =
				IGraphPinVisualizerInterface::Execute_GetSupportedGraphPinTag(PinVisualizerClass->GetDefaultObject());
			SupportedTag.IsValid())
		{
			if (auto&& ObjPtrs = PinVisualizerMap.Find(SupportedTag))
			{
				if (auto&& ObjPtr = ObjPtrs->Find(PinVisualizerClass.Get()))
				{
					if ((--*ObjPtr).Count == 0)
					{
						ObjPtrs->Remove(*ObjPtr);
						if (ObjPtrs->IsEmpty())
						{
							PinVisualizerMap.Remove(SupportedTag);
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

void UHeartGraphNodeRegistry::ForEachNodeObjectClass(const TFunctionRef<bool(TSubclassOf<UHeartGraphNode>, FHeartNodeSource)>& Iter) const
{
	for (auto It : Heart::Query::FRegistryQueryResult::FRange(this))
	{
		checkSlow(It.Value.GraphNode.Get());
		checkSlow(It.Value.Source.IsValid());
		if (!Iter(It.Value.GraphNode.Get(), It.Value.Source))
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
		[&UnsortedCategories](const TSubclassOf<UHeartGraphNode>& GraphNodeClass, const FHeartNodeSource& NodeSource)
		{
			const UHeartGraphNode* GraphNodeCDO = GraphNodeClass->GetDefaultObject<UHeartGraphNode>();
			const UObject* NodeSourceCDO = NodeSource.GetDefaultObject();

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

void UHeartGraphNodeRegistry::GetAllGraphNodeClassesAndNodeSources(
	TMap<FHeartNodeSource, TSubclassOf<UHeartGraphNode>>& OutClasses) const
{
	OutClasses.Empty(GetNumNodes(true));
	for (const TTuple<FHeartNodeSource, FNodeSourceEntry>& Element : NodeRootTable)
	{
		for (auto&& GraphNodePtr : Element.Value.GraphNodes)
		{
			auto&& GraphNode = GraphNodePtr.Obj.Get();
			if (!IsValid(GraphNode))
			{
				continue;
			}

			OutClasses.Add(Element.Key, GraphNode);
			for (TObjectPtr<UClass> Child : Element.Value.RecursiveChildren)
			{
				OutClasses.Add(FHeartNodeSource(Child), GraphNode);
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
			for (auto&& FallbackClass : Fallback->GetRegistrationList().NodeVisualizerClasses)
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
			for (auto&& FallbackClass : Fallback->GetRegistrationList().PinVisualizerClasses)
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

	UE_LOG(LogHeartNodeRegistry, Warning, TEXT("Registry was unable to find a pin visualizer for Tag '%s'"), *GraphPinDesc.Tag.GetTagName().ToString())

	return nullptr;
}

UClass* UHeartGraphNodeRegistry::GetVisualizerClassForGraphConnection(const FHeartGraphPinDesc& FromPinDesc,
																	  const FHeartGraphPinDesc& ToPinDesc,
																	  UClass* VisualizerBase) const
{
	// @todo add ability to override the connection class to anything other than the default
	// @todo also note that either From or To may be invalid in case of drawing a preview connection

	// Try and retrieve a fallback visualizer
	if (auto&& Subsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		if (auto&& Fallback = Subsystem->GetFallbackRegistrar())
		{
			for (auto&& FallbackClass : Fallback->GetRegistrationList().ConnectionVisualizerClasses)
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