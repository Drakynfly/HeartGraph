// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "GraphRegistry/GraphNodeRegistrar.h"
#include "GraphRegistry/HeartRegistryQuery.h"
#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#include "GraphRegistry/HeartRegistryStructs.h"
#include "Model/HeartGraphNode.h"
#include "View/HeartVisualizerInterfaces.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNodeRegistry)

void UHeartGraphNodeRegistry::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(InThis, Collector);
	UHeartGraphNodeRegistry* This = CastChecked<ThisClass>(InThis);
	for (auto&& Element : This->NodeRootTable)
	{
		Element.Key.AddStructReferencedObjects(Collector);
		Element.Value.NodeClasses.CollectReferences(Collector);
		Collector.AddStableReferenceArray(&Element.Value.RecursiveChildren);
	}
	This->NodeVisualizers.CollectReferences(Collector);
	This->PinVisualizers.CollectReferences(Collector);
	This->ConnectionVisualizers.CollectReferences(Collector);
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

void UHeartGraphNodeRegistry::GatherReferences(const FHeartRegistryClassLists& List, TArray<FSoftObjectPath>& Objects)
{
	auto AddIfPending = [&Objects]<typename T>(const T& SoftPtr)
		{
			if (SoftPtr.IsPending())
			{
				Objects.Add(SoftPtr.ToSoftObjectPath());
			}
		};

	for (auto&& NodeList : List.NodeLists)
	{
		AddIfPending(NodeList.Key);
		for (auto&& Class : NodeList.Value.Classes)
		{
			AddIfPending(Class.Class);
		}
		for (auto&& Object : NodeList.Value.Objects)
		{
			AddIfPending(Object);
		}
		for (auto&& Visualizer : NodeList.Value.Visualizers)
		{
			AddIfPending(Visualizer);
		}
	}
	for (auto&& PinList : List.PinLists)
	{
		for (auto&& PinVisualizer : PinList.Value.PinVisualizers)
		{
			AddIfPending(PinVisualizer);
		}
		for (auto&& ConnectionVisualizer : PinList.Value.ConnectionVisualizers)
		{
			AddIfPending(ConnectionVisualizer);
		}
	}
}

void UHeartGraphNodeRegistry::AddRegistrationList(const FHeartRegistryClassLists& List, const bool Broadcast)
{
	for (auto&& NodeList : List.NodeLists)
	{
		UClass* GraphNodeClass = NodeList.Key.Get();

		if (!FilterObjectForRegistration(GraphNodeClass))
		{
			continue;
		}

		for (const FHeartRegistryClass& Element : NodeList.Value.Classes)
		{
			UClass* Class = Element.Class.Get();

			if (!FilterObjectForRegistration(Class))
			{
				continue;
			}

			FNodeSourceEntry& Entry = NodeRootTable.FindOrAdd(FHeartNodeSource(Class));

			Entry.NodeClasses.Inc(GraphNodeClass);

			if (Element.RegisterAssets)
			{
				TArray<FAssetData> AssetData;
				IAssetRegistry::Get()->GetAssetsByClass(Class->GetClassPathName(), AssetData);

				TArray<FSoftObjectPath> ObjectPaths;
				ObjectPaths.Reserve(AssetData.Num());

				for (auto&& Asset : AssetData)
				{
					if (Asset.IsAssetLoaded())
					{
						NodeRootTable.FindOrAdd(FHeartNodeSource(Asset.GetAsset())).NodeClasses.Inc(GraphNodeClass);
					}
					else
					{
						ObjectPaths.Add(Asset.GetSoftObjectPath());
					}
				}

				// @todo allow this to be async
				if (!ObjectPaths.IsEmpty())
				{
					UAssetManager::GetStreamableManager().RequestSyncLoad(ObjectPaths);
					for (auto&& ObjectPath : ObjectPaths)
					{
						if (UObject* Obj = ObjectPath.ResolveObject();
							IsValid(Obj))
						{
							NodeRootTable.FindOrAdd(FHeartNodeSource(Obj)).NodeClasses.Inc(GraphNodeClass);
						}
					}
				}
			}

			// Recursive elements increment a Recursivity counter that gathers derived classes while > 0
			if (Element.Recursive)
			{
				if (Entry.RecursiveRegistryCounter == 0)
				{
					GetDerivedClasses(Class, MutableView(Entry.RecursiveChildren));
				}

				Entry.RecursiveRegistryCounter++;
			}
		}

		for (auto&& Element : NodeList.Value.Objects)
		{
			UObject* Object = Element.Get();

			if (!FilterObjectForRegistration(Object))
			{
				continue;
			}

			NodeRootTable.FindOrAdd(FHeartNodeSource(Object)).NodeClasses.Inc(GraphNodeClass);
		}

		for (auto&& NodeVisualizer : NodeList.Value.Visualizers)
		{
			if (!FilterObjectForRegistration(NodeVisualizer.Get()))
			{
				continue;
			}

			NodeVisualizers.Inc(GraphNodeClass, NodeVisualizer.Get());
		}
	}

	for (auto&& Element : List.PinLists)
	{
		if (!Element.Key.IsValid() ||
			Element.Key == FHeartGraphPinTag::GetRootTag())
		{
			continue;
		}

		for (auto&& PinVisualizer : Element.Value.PinVisualizers)
		{
			if (!FilterObjectForRegistration(PinVisualizer.Get()))
			{
				continue;
			}

			PinVisualizers.Inc(Element.Key, PinVisualizer.Get());
		}

		for (auto&& ConnectionVisualizerClass : Element.Value.ConnectionVisualizers)
		{
			if (!FilterObjectForRegistration(ConnectionVisualizerClass.Get()))
			{
				continue;
			}

			ConnectionVisualizers.Inc(Element.Key, ConnectionVisualizerClass.Get());
		}
	}

	if (Broadcast)
	{
		BroadcastChange();
	}
}

void UHeartGraphNodeRegistry::RemoveRegistrationList(const FHeartRegistryClassLists& List, const bool Broadcast)
{
	auto DecrementSource = [&](UClass* Key, const FHeartNodeSource& Src)
		{
			if (auto* Entry = NodeRootTable.Find(Src))
			{
				Entry->NodeClasses.Dec(Key);
				if (Entry->NodeClasses.IsEmpty())
				{
					NodeRootTable.Remove(Src);
				}
			}
		};

	for (const auto& NodeList : List.NodeLists)
	{
		UClass* GraphNodeClass = NodeList.Key.Get();
		if (!IsValid(GraphNodeClass))
		{
			continue;
		}

		for (const FHeartRegistryClass& Element : NodeList.Value.Classes)
		{
			const FHeartNodeSource SrcObj(Element.Class.Get());

			DecrementSource(GraphNodeClass, SrcObj);

			if (Element.RegisterAssets)
			{
				// We don't need to load these. If they are registered, then they should already be loaded.
				TArray<FAssetData> AssetData;
				IAssetRegistry::Get()->GetAssetsByClass(GraphNodeClass->GetClassPathName(), AssetData);
				for (auto&& Asset : AssetData)
				{
					if (Asset.IsAssetLoaded())
					{
						DecrementSource(GraphNodeClass, FHeartNodeSource(Asset.GetAsset()));
					}
				}
			}

			// Recursive Elements decrement their own counter, and remove RecursiveChildren at 0.
			if (Element.Recursive)
			{
				auto* Entry = NodeRootTable.Find(SrcObj);
				if (Entry == nullptr) continue;

				checkSlow(Entry->RecursiveRegistryCounter > 0);
				if (--Entry->RecursiveRegistryCounter == 0)
				{
					Entry->RecursiveChildren.Empty();
				}
			}
		}

		for (auto&& Object : NodeList.Value.Objects)
		{
			if (UObject* Obj = Object.Get();
				IsValid(Obj))
			{
				DecrementSource(GraphNodeClass, FHeartNodeSource(Obj));
			}
		}

		for (auto&& NodeVisualizer : NodeList.Value.Visualizers)
		{
			NodeVisualizers.Dec(GraphNodeClass, NodeVisualizer.Get());
		}
	}

	for (auto&& Element : List.PinLists)
	{
		if (Element.Key.IsValid())
		{
			continue;
		}

		for (auto&& PinVisualizer : Element.Value.PinVisualizers)
		{
			PinVisualizers.Dec(Element.Key, PinVisualizer.Get());
		}

		for (auto&& ConnectionVisualizer : Element.Value.ConnectionVisualizers)
		{
			ConnectionVisualizers.Dec(Element.Key, ConnectionVisualizer.Get());
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
	for (Heart::Query::FRegistryPair&& It : Heart::Query::FRegistryQueryResult::FRange(this))
	{
		if (!ensure(IsValid(It.Value.GraphNode)) ||
			!ensure(It.Value.Source.IsValid()))
		{
			continue;
		}

		if (!Iter(It.Value))
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
		for (auto&& GraphNodePtr : Element.Value.NodeClasses.Classes)
		{
			auto&& GraphNode = GraphNodePtr.Obj;
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
			return Entry->NodeClasses.Classes.Get(FSetElementId::FromInteger(0)).Obj;
		}
	}

	return nullptr;
}

TArray<TSubclassOf<UHeartGraphNode>> UHeartGraphNodeRegistry::GetGraphNodeClassesForNode(const FHeartNodeSource NodeSource) const
{
	// Cursed for-loop, but it works :)
	for (FHeartNodeSource Test = NodeSource;
		Test.IsValid() && Test != FHeartNodeSource(UObject::StaticClass());
		Test = FHeartNodeSource(Test.NextClass()))
	{
		if (const FNodeSourceEntry* Entry = NodeRootTable.Find(Test))
		{
			TArray<TSubclassOf<UHeartGraphNode>> GraphNodes;
			for (auto&& GraphNode : Entry->NodeClasses.Classes)
			{
				if (IsValid(GraphNode.Obj))
				{
					GraphNodes.Add(GraphNode.Obj);
				}
			}
			return GraphNodes;
		}
	}

	return {};
}

UClass* UHeartGraphNodeRegistry::GetVisualizerClassForGraphNode(const TSubclassOf<UHeartGraphNode> GraphNodeClass, UClass* VisualizerBase) const
{
	if (!IsValid(GraphNodeClass))
	{
		UE_LOG(LogHeartNodeRegistry, Warning, TEXT("Invalid class passed to GetVisualizerClassForGraphNode!"))
		return nullptr;
	}

	if (!NodeVisualizers.IsEmpty())
	{
		for (auto TestClass = GraphNodeClass;
			IsValid(TestClass) && TestClass != UObject::StaticClass();
			TestClass = TestClass->GetSuperClass())
		{
			if (IsValid(VisualizerBase))
			{
				if (auto&& Class = NodeVisualizers.FindByClass(TestClass, VisualizerBase))
				{
					return Class;
				}
			}
			else
			{
				if (auto&& Class = NodeVisualizers.Find(TestClass))
				{
					return Class;
				}
			}
		}
	}

	// Try and retrieve a fallback visualizer
	if (auto&& Subsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		if (auto&& Fallback = Subsystem->GetFallbackRegistrar())
		{
			for (auto&& GraphNodeList : Fallback->GetClassLists().NodeLists)
			{
				for (auto&& Element : GraphNodeList.Value.Visualizers)
				{
					ensure(IsValid(Element.LoadSynchronous()));

					if (!IsValid(VisualizerBase) ||
						Element->IsChildOf(VisualizerBase))
					{
						return Element.Get();
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
	if (!PinVisualizers.IsEmpty())
	{
		for (FHeartGraphPinTag Tag = GraphPinDesc.Tag; Tag.IsValid() && Tag != FHeartGraphPinTag::GetRootTag();
				Tag = FHeartGraphPinTag::TryConvert(Tag.RequestDirectParent()))
		{
			if (IsValid(VisualizerBase))
			{
				if (auto&& Class = PinVisualizers.FindByClass(Tag, VisualizerBase))
				{
					return Class;
				}
			}
			else
			{
				if (auto&& Class = PinVisualizers.Find(Tag))
				{
					return Class;
				}
			}
		}
	}

	// Try and retrieve a fallback visualizer
	if (auto&& Subsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		if (auto&& Fallback = Subsystem->GetFallbackRegistrar())
		{
			for (auto&& GraphPinList : Fallback->GetClassLists().PinLists)
			{
				for (auto&& FallbackClass : GraphPinList.Value.PinVisualizers)
				{
					ensure(IsValid(FallbackClass.LoadSynchronous()));

					if (!IsValid(VisualizerBase) ||
						FallbackClass->IsChildOf(VisualizerBase))
					{
						return FallbackClass.Get();
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

	if (!ConnectionVisualizers.IsEmpty())
	{
		for (; SearchTag.IsValid() && SearchTag != FHeartGraphPinTag::GetRootTag();
				SearchTag = FHeartGraphPinTag::TryConvert(SearchTag.RequestDirectParent()))
		{
			if (IsValid(VisualizerBase))
			{
				if (auto&& Class = ConnectionVisualizers.FindByClass(SearchTag, VisualizerBase))
				{
					return Class;
				}
			}
			else
			{
				if (auto&& Class = ConnectionVisualizers.Find(SearchTag))
				{
					return Class;
				}
			}
		}
	}

	// Try and retrieve a fallback visualizer
	if (auto&& Subsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		if (auto&& Fallback = Subsystem->GetFallbackRegistrar())
		{
			for (auto&& GraphPinList : Fallback->GetClassLists().PinLists)
			{
				for (auto&& FallbackClass : GraphPinList.Value.ConnectionVisualizers)
				{
					ensure(IsValid(FallbackClass.LoadSynchronous()));

					if (!IsValid(VisualizerBase) ||
						FallbackClass->IsChildOf(VisualizerBase))
					{
						return FallbackClass.Get();
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