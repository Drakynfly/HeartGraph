// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "GraphRegistry/GraphNodeRegistrar.h"
#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#include "Model/HeartGraphNode.h"
#include "View/HeartVisualizerInterfaces.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNodeRegistry)

namespace Heart
{
	struct FFilterLambdas
	{
		using FGraphNodeTest = TFunction<bool(const TSubclassOf<UHeartGraphNode>)>;
		using FNodeSourceTest = TFunction<bool(const FHeartNodeSource)>;

		explicit FFilterLambdas(const FHeartRegistryQuery& Query)
		{
			FilterMaxResults = Query.MaxResults > 0 ? Query.MaxResults : TNumericLimits<int32>::Max();

			GraphNodeClassTest = IsValid(Query.HeartGraphNodeBaseClass) ?
				FGraphNodeTest([&Query](const TSubclassOf<UHeartGraphNode> GraphNodeClass)
				{
					return GraphNodeClass->IsChildOf(Query.HeartGraphNodeBaseClass);
				}) :
				FGraphNodeTest([](const TSubclassOf<UHeartGraphNode>)
				{
					return true;
				});

			NodeSourceClassTest = IsValid(Query.NodeObjectBaseClass) ?
				FNodeSourceTest([&Query](const FHeartNodeSource NodeSource)
				{
					return NodeSource.IsAOrClassOf(Query.NodeObjectBaseClass);
				}) :
				FNodeSourceTest([](const FHeartNodeSource)
				{
					return true;
				});

			CustomFilterTest = Query.Filter.IsBound() ?
				FNodeSourceTest([&Query](const FHeartNodeSource NodeSource)
				{
					return Query.Filter.Execute(NodeSource);
				}) :
				FNodeSourceTest([](const FHeartNodeSource)
				{
					return true;
				});
		}

		int32 FilterMaxResults;
		FGraphNodeTest GraphNodeClassTest;
		FNodeSourceTest NodeSourceClassTest;
		FNodeSourceTest CustomFilterTest;
	};
}

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
		if (GraphNodeList.Value.Classes.IsEmpty())
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
			Entry.SelfRegistryCounter++;

			if (FilterObjectForRegistration(GraphNodeList.Key))
			{
				// @todo this kinda obliterates any previous value! what to do??
				Entry.GraphNode = GraphNodeList.Key;
			}

			if (Element.Recursive)
			{
				if (Entry.RecursiveRegistryCounter == 0)
				{
					GetDerivedClasses(Element.Class, Entry.RecursiveChildren);
				}

				Entry.RecursiveRegistryCounter++;
			}
		}
	}

	for (auto&& NodeObjectList : Registration.IndividualObjects)
	{
		if (NodeObjectList.Value.Objects.IsEmpty())
		{
			continue;
		}

		for (auto&& NodeObject : NodeObjectList.Value.Objects)
		{
			if (!FilterObjectForRegistration(NodeObject))
			{
				continue;
			}

			FNodeSourceEntry& Entry = NodeRootTable.FindOrAdd(FHeartNodeSource(NodeObject));
			Entry.SelfRegistryCounter++;

			if (FilterObjectForRegistration(NodeObjectList.Key))
			{
				// @todo this kinda obliterates any previous value! what to do??
				Entry.GraphNode = NodeObjectList.Key;
			}
		}
	}

	for (auto&& NodeVisualizerClass : Registration.NodeVisualizerClasses)
	{
		if (!FilterObjectForRegistration(NodeVisualizerClass))
		{
			continue;
		}

		if (UClass* SupportedClass =
				IGraphNodeVisualizerInterface::Execute_GetSupportedGraphNodeClass(NodeVisualizerClass->GetDefaultObject()))
		{
			NodeVisualizerMap.FindOrAdd(SupportedClass).FindOrAdd(NodeVisualizerClass)++;
		}
	}

	for (auto&& PinVisualizerClass : Registration.PinVisualizerClasses)
	{
		if (!FilterObjectForRegistration(PinVisualizerClass))
		{
			continue;
		}

		FHeartGraphPinTag SupportedTag =
			IGraphPinVisualizerInterface::Execute_GetSupportedGraphPinTag(PinVisualizerClass->GetDefaultObject());
		if (SupportedTag.IsValid())
		{
			PinVisualizerMap.FindOrAdd(SupportedTag).FindOrAdd(PinVisualizerClass)++;
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

			if (--Entry->SelfRegistryCounter == 0)
			{
				NodeRootTable.Remove(SrcClass);
			}
			else if (Object.Recursive)
			{
				if (--Entry->RecursiveRegistryCounter == 0)
				{
					Entry->RecursiveChildren.Empty();
				}
			}
		}
	}

	for (const TTuple<TSubclassOf<UHeartGraphNode>, FHeartObjectList>& Element : Registration.IndividualObjects)
	{
		for (auto Object : Element.Value.Objects)
		{
			FHeartNodeSource SrcObj(Object);

			auto* Entry = NodeRootTable.Find(SrcObj);
			if (Entry == nullptr) continue;

			if (--Entry->SelfRegistryCounter == 0)
			{
				NodeRootTable.Remove(SrcObj);
			}
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

		FHeartGraphPinTag SupportedTag =
			IGraphPinVisualizerInterface::Execute_GetSupportedGraphPinTag(PinVisualizerClass->GetDefaultObject());
		if (SupportedTag.IsValid())
		{
			PinVisualizerMap.Remove(SupportedTag);
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
#if WITH_EDITOR
	FEditorScriptExecutionGuard EditorScriptExecutionGuard;
#endif
	OnRegistryChangedNative.Broadcast(this);
	OnRegistryChanged.Broadcast(this);
}

bool UHeartGraphNodeRegistry::IsRegistered(const UGraphNodeRegistrar* Registrar) const
{
	return ContainedRegistrars.Contains(Registrar);
}

void UHeartGraphNodeRegistry::ForEachNodeObjectClass(const TFunctionRef<bool(TSubclassOf<UHeartGraphNode>, FHeartNodeSource)>& Iter) const
{
	for (const TTuple<FHeartNodeSource, FNodeSourceEntry>& Element : NodeRootTable)
	{
		TSubclassOf<UHeartGraphNode> GraphNodeClass = Element.Value.GraphNode;
		if (!ensure(IsValid(GraphNodeClass))) continue;

		if (!Iter(GraphNodeClass, Element.Key))
		{
			return;
		}

		for (const UClass* Child : Element.Value.RecursiveChildren)
		{
			FHeartNodeSource NodeSource(Child);
			if (!ensure(NodeSource.IsValid())) continue;

			if (!Iter(GraphNodeClass, NodeSource))
			{
				return;
			}
		}
	}
}

TArray<FString> UHeartGraphNodeRegistry::GetNodeCategories() const
{
	TSet<FString> UnsortedCategories;

	ForEachNodeObjectClass(
		[&UnsortedCategories](const TSubclassOf<UHeartGraphNode> GraphNodeClass, const FHeartNodeSource NodeSource)
		{
			const UHeartGraphNode* GraphNodeCDO = GraphNodeClass->GetDefaultObject<UHeartGraphNode>();
			const UObject* NodeSourceCDO = NodeSource.GetDefaultObject();

			UnsortedCategories.Emplace(GraphNodeCDO->GetNodeCategory(NodeSourceCDO).ToString());

			return true;
		});

	TArray<FString> SortedCategories = UnsortedCategories.Array();
	SortedCategories.Sort();
	return SortedCategories;
}

void UHeartGraphNodeRegistry::GetAllNodeSources(TArray<FHeartNodeSource>& OutNodeSources) const
{
	for (const TTuple<FHeartNodeSource, FNodeSourceEntry>& Element : NodeRootTable)
	{
		OutNodeSources.Add(Element.Key);
		OutNodeSources.Append(Element.Value.RecursiveChildren);
	}
}

void UHeartGraphNodeRegistry::GetAllGraphNodeClassesAndNodeSources(
	TMap<FHeartNodeSource, TSubclassOf<UHeartGraphNode>>& OutClasses) const
{
	for (const TTuple<FHeartNodeSource, FNodeSourceEntry>& Element : NodeRootTable)
	{
		OutClasses.Add(Element.Key, Element.Value.GraphNode);
		for (TObjectPtr<UClass> Child : Element.Value.RecursiveChildren)
		{
			OutClasses.Add(FHeartNodeSource(Child), Element.Value.GraphNode);
		}
	}
}

void UHeartGraphNodeRegistry::QueryNodeClasses(const FHeartRegistryQuery& Query, TArray<FHeartNodeSource>& OutNodeSources) const
{
	Heart::FFilterLambdas FilterLambdas(Query);

	ForEachNodeObjectClass(
		[&OutNodeSources, &FilterLambdas](const TSubclassOf<UHeartGraphNode> GraphNodeClass, const FHeartNodeSource NodeSource)
		{
			if (FilterLambdas.GraphNodeClassTest(GraphNodeClass) &&
				FilterLambdas.NodeSourceClassTest(NodeSource) &&
				FilterLambdas.CustomFilterTest(NodeSource))
			{
				OutNodeSources.Add(NodeSource);
				return OutNodeSources.Num() < FilterLambdas.FilterMaxResults;
			}

			return true;
		});

	if (Query.Sort.IsBound())
	{
		Algo::Sort(OutNodeSources,
			[Callback = Query.Sort](const FHeartNodeSource A, const FHeartNodeSource B)
			{
				return Callback.Execute(A, B);
			});
	}
	else if (Query.Score.IsBound())
	{
		TMap<FHeartNodeSource, double> Scores;
		Scores.Reserve(OutNodeSources.Num());
		for (FHeartNodeSource Class : OutNodeSources)
		{
			Scores.Add({Class, Query.Score.Execute(Class)});
		}

		Algo::Sort(OutNodeSources,
			[&Scores](const FHeartNodeSource A, const FHeartNodeSource B)
			{
				return Scores[A] < Scores[B];
			});
	}
}

void UHeartGraphNodeRegistry::QueryGraphAndNodeClasses(const FHeartRegistryQuery& Query,
	TMap<FHeartNodeSource, TSubclassOf<UHeartGraphNode>>& OutClasses) const
{
	Heart::FFilterLambdas FilterLambdas(Query);

	ForEachNodeObjectClass(
		[&OutClasses, &FilterLambdas](const TSubclassOf<UHeartGraphNode> GraphNodeClass, const FHeartNodeSource NodeSource)
		{
			if (FilterLambdas.GraphNodeClassTest(GraphNodeClass) &&
				FilterLambdas.NodeSourceClassTest(NodeSource) &&
				FilterLambdas.CustomFilterTest(NodeSource))
			{
				OutClasses.Add(NodeSource, GraphNodeClass);
				return OutClasses.Num() < FilterLambdas.FilterMaxResults;
			}

			return true;
		});

	if (Query.Sort.IsBound())
	{
		OutClasses.KeySort(
			[Callback = Query.Sort](const FHeartNodeSource A, const FHeartNodeSource B)
			{
				return Callback.Execute(A, B);
			});
	}
	else if (Query.Score.IsBound())
	{
		TMap<FHeartNodeSource, double> Scores;
		Scores.Reserve(OutClasses.Num());
		for (const TTuple<FHeartNodeSource, TSubclassOf<UHeartGraphNode>>& ClassPair : OutClasses)
		{
			Scores.Add({ClassPair.Key, Query.Score.Execute(ClassPair.Key)});
		}

		OutClasses.KeySort(
			[&Scores](const FHeartNodeSource A, const FHeartNodeSource B)
			{
				return Scores[A] < Scores[B];
			});
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
			return Entry->GraphNode;
		}
	}

	return nullptr;
}

UClass* UHeartGraphNodeRegistry::GetVisualizerClassForGraphNode(const TSubclassOf<UHeartGraphNode> GraphNodeClass, UClass* VisualizerBase) const
{
	for (UClass* Class = GraphNodeClass; Class && Class != UObject::StaticClass(); Class = Class->GetSuperClass())
	{
		if (auto&& ClassMap = NodeVisualizerMap.Find(Class))
		{
			for (auto&& CountedClass : *ClassMap)
			{
				if (!IsValid(VisualizerBase))
				{
					return CountedClass.Key;
				}

				if (CountedClass.Key->IsChildOf(VisualizerBase))
				{
					return CountedClass.Key;
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

				if (!IsValid(VisualizerBase) || FallbackClass->IsChildOf(VisualizerBase))
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
					if (!IsValid(VisualizerBase))
					{
						return CountedClass.Key;
					}

					if (CountedClass.Key->IsChildOf(VisualizerBase))
					{
						return CountedClass.Key;
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

				if (!IsValid(VisualizerBase) || FallbackClass->IsChildOf(VisualizerBase))
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

				if (!IsValid(VisualizerBase) || FallbackClass->IsChildOf(VisualizerBase))
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