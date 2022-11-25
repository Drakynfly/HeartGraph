// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartNodeRegistrySubsystem.h"

#include "Model/GraphNodeRegistrar.h"
#include "ModelView/HeartGraphNode.h"
#include "ModelView/HeartGraphNodeBlueprint.h"
#include "ModelView/HeartGraphPin.h"
#include "View/HeartVisualizerInterfaces.h"

#include "AssetRegistry/AssetRegistryModule.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

DEFINE_LOG_CATEGORY(LogHeartNodeRegistry)

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
	NodeClasses.Append(Registration.NodeClasses);

	for (auto&& GraphNodeClass : Registration.GraphNodeClasses)
	{
		if (IsValid(GraphNodeClass))
		{
			if (UClass* SupportedClass = GetDefault<UHeartGraphNode>(GraphNodeClass)->GetSupportedClass())
			{
				GraphNodeMap.Add(SupportedClass, GraphNodeClass);
			}
		}
	}

	for (auto&& NodeVisualizerClass : Registration.NodeVisualizerClasses)
	{
		if (auto&& Class = NodeVisualizerClass.LoadSynchronous())
		{
			if (UClass* SupportedClass =
					IGraphNodeVisualizerInterface::Execute_GetSupportedGraphNodeClass(NodeVisualizerClass->GetDefaultObject()))
			{
				NodeVisualizerMap.Add(SupportedClass, Class);
			}
		}
	}

	for (auto&& PinVisualizerClass : Registration.PinVisualizerClasses)
	{
		if (auto&& Class = PinVisualizerClass.LoadSynchronous())
		{
			if (UClass* SupportedClass =
				   IGraphPinVisualizerInterface::Execute_GetSupportedGraphPinClass(PinVisualizerClass->GetDefaultObject()))
			{
				PinVisualizerMap.Add(SupportedClass, Class);
			}
		}
	}
}

void UHeartGraphNodeRegistry::RemoveRegistrationList(const FHeartRegistrationClasses& Registration)
{
	for (auto&& NodeClass : Registration.NodeClasses)
	{
		NodeClasses.Remove(NodeClass);
	}

	for (auto&& GraphNodeClass : Registration.GraphNodeClasses)
	{
		if (UClass* SupportedClass = GetDefault<UHeartGraphNode>(GraphNodeClass)->GetSupportedClass())
		{
			GraphNodeMap.Remove(SupportedClass);
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

void UHeartGraphNodeRegistry::GetFilteredNodeClasses(const FNativeNodeClassFilter& Filter,
	TArray<UClass*>& OutClasses) const
{
	if (!ensure(Filter.IsBound()))
	{
		return;
	}

	for (auto&& NodeClass : NodeClasses)
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

void UHeartGraphNodeRegistry::GetFilteredNodeClasses(const FNodeClassFilter& Filter, TArray<UClass*>& OutClasses) const
{
	if (!ensure(Filter.IsBound()))
	{
		return;
	}

	for (auto&& NodeClass : NodeClasses)
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

bool UHeartNodeRegistrySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if WITH_EDITOR
	return Super::ShouldCreateSubsystem(Outer);
#else
	return GetDefault<UHeartGraphSettings>()->CreateRuntimeNodeRegistry;
#endif
}

void UHeartNodeRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FetchNativeClasses();

#if WITH_EDITOR
	const FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
	AssetRegistry.Get().OnFilesLoaded().AddUObject(this, &ThisClass::OnFilesLoaded);
	AssetRegistry.Get().OnAssetAdded().AddUObject(this, &ThisClass::OnAssetAdded);
	AssetRegistry.Get().OnAssetRemoved().AddUObject(this, &ThisClass::OnAssetRemoved);

	FCoreUObjectDelegates::ReloadCompleteDelegate.AddUObject(this, &ThisClass::OnHotReload);

	if (GEditor)
	{
		GEditor->OnBlueprintPreCompile().AddUObject(this, &ThisClass::OnBlueprintPreCompile);
		GEditor->OnBlueprintCompiled().AddUObject(this, &ThisClass::OnBlueprintCompiled);
	}
#endif
}

#if WITH_EDITOR
void UHeartNodeRegistrySubsystem::OnFilesLoaded()
{
	FetchAssetRegistryAssets();
}

void UHeartNodeRegistrySubsystem::OnAssetAdded(const FAssetData& AssetData)
{
	if (auto&& AssetClass = AssetData.GetClass())
	{
		if (AssetClass->IsChildOf(UGraphNodeRegistrar::StaticClass()))
		{
			auto&& NewRegistrar = Cast<UGraphNodeRegistrar>(AssetData.GetAsset());

			if (NewRegistrar && NewRegistrar->AutoRegister)
			{
				AddRegistrar(NewRegistrar);
			}
		}
	}
}

void UHeartNodeRegistrySubsystem::OnAssetRemoved(const FAssetData& AssetData)
{
	if (AssetData.GetClass()->IsChildOf(UGraphNodeRegistrar::StaticClass()))
	{
		auto&& NewRegistrar = Cast<UGraphNodeRegistrar>(AssetData.GetAsset());

		if (NewRegistrar)
		{
			RemoveRegistrar(NewRegistrar);
		}
	}
}

void UHeartNodeRegistrySubsystem::OnHotReload(EReloadCompleteReason ReloadCompleteReason)
{
	FetchAssetRegistryAssets();
}

void UHeartNodeRegistrySubsystem::OnBlueprintPreCompile(UBlueprint* Blueprint)
{
	if (Blueprint && Blueprint->GeneratedClass && Blueprint->GeneratedClass->IsChildOf(UHeartGraphNode::StaticClass()))
	{
		WaitingForBlueprintToCompile++;
	}
}

void UHeartNodeRegistrySubsystem::OnBlueprintCompiled()
{
	WaitingForBlueprintToCompile--;

	if (WaitingForBlueprintToCompile <= 0)
	{
		WaitingForBlueprintToCompile = 0;
		FetchAssetRegistryAssets();
	}
}

#endif

UBlueprint* UHeartNodeRegistrySubsystem::GetNodeBlueprint(const FAssetData& AssetData)
{
	UBlueprint* Blueprint = Cast<UBlueprint>(AssetData.GetAsset());
	if (Blueprint && (Blueprint->GeneratedClass))
	{
		return Blueprint;
	}

	return nullptr;
}

void UHeartNodeRegistrySubsystem::FetchNativeClasses()
{
	TArray<UClass*> HeartGraphNodeClasses;
	GetDerivedClasses(UHeartGraphNode::StaticClass(), HeartGraphNodeClasses);
	for (UClass* Class : HeartGraphNodeClasses)
	{
		if (Class->IsNative())
		{
			KnownNativeClasses.GraphNodeClasses.Emplace(Class);
		}
	}

	// @todo PLEASE FIND A BETTER WAY TO DO THIS

	TArray<UClass*> NodeVisualizerClasses;
	GetDerivedClasses(UObject::StaticClass(), NodeVisualizerClasses);
	for (UClass* Class : NodeVisualizerClasses)
	{
		if (Class->IsNative() && Class->ImplementsInterface(UGraphNodeVisualizerInterface::StaticClass()))
		{
			KnownNativeClasses.NodeVisualizerClasses.Emplace(Class);
		}
	}

	TArray<UClass*> PinVisualizerClasses;
	GetDerivedClasses(UObject::StaticClass(), PinVisualizerClasses);
	for (UClass* Class : PinVisualizerClasses)
	{
		if (Class->IsNative() && Class->ImplementsInterface(UGraphPinVisualizerInterface::StaticClass()))
		{
			KnownNativeClasses.PinVisualizerClasses.Emplace(Class);
		}
	}

	// @todo we cannot autodiscover native classes of UHeartEdGraphNode as they are in the editor module
	// To fix this we would need to have the editor module inject them somehow, but preferable we wouldn't do this at all
	/*
	TArray<UClass*> GraphNodes;
	GetDerivedClasses(UHeartEdGraphNode::StaticClass(), GraphNodes);
	for (UClass* Class : GraphNodes)
	{
		auto&& DefaultObject = Class->GetDefaultObject<UHeartEdGraphNode>();
		for (UClass* AssignedClass : DefaultObject->AssignedNodeClasses)
		{
			if (AssignedClass->IsChildOf(UHeartGraphNode::StaticClass()))
			{
				AssignedGraphNodeClasses.Emplace(AssignedClass, Class);
			}
		}
	}
	*/

}

void UHeartNodeRegistrySubsystem::FetchAssetRegistryAssets()
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);

	FARFilter RegistrarFilter;
	RegistrarFilter.ClassPaths.Add(UGraphNodeRegistrar::StaticClass()->GetClassPathName());
	RegistrarFilter.bRecursiveClasses = true;

	TArray<FAssetData> FoundRegistrarAssets;
	AssetRegistryModule.Get().GetAssets(RegistrarFilter, FoundRegistrarAssets);
	for (const FAssetData& RegistrarAsset : FoundRegistrarAssets)
	{
		auto&& Registrar = Cast<UGraphNodeRegistrar>(RegistrarAsset.GetAsset());

		if (Registrar && Registrar->AutoRegister)
		{
			AddRegistrar(Registrar);
		}
	}

	FARFilter NodeClassFilter;
	NodeClassFilter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
	NodeClassFilter.ClassPaths.Add(UBlueprintGeneratedClass::StaticClass()->GetClassPathName());
	NodeClassFilter.bRecursiveClasses = true;

	TArray<FAssetData> FoundAssets;
	AssetRegistryModule.Get().GetAssets(RegistrarFilter, FoundAssets);
	for (const FAssetData& AssetData : FoundAssets)
	{
		if (AssetData.GetClass()->IsChildOf(UHeartGraphNodeBlueprint::StaticClass()))
		{
			if (!KnownBlueprintHeartGraphNodes.Contains(AssetData.PackageName))
			{
				KnownBlueprintHeartGraphNodes.Add(AssetData.PackageName, AssetData);

				for (auto&& NodeRegistry : NodeRegistries)
				{
					if (auto&& Blueprint = GetNodeBlueprint(AssetData))
					{
						//NodeRegistry.Value->NotifyNodeBlueprintNodeClassAdded({Blueprint->GeneratedClass});
					}
				}
			}
		}
	}
}

UHeartGraphNodeRegistry* UHeartNodeRegistrySubsystem::GetRegistry(const TSubclassOf<UHeartGraph> Class)
{
	if (auto&& FoundRegistry = NodeRegistries.Find(Class))
	{
		return *FoundRegistry;
	}

	auto&& NewRegistry = NewObject<UHeartGraphNodeRegistry>(this);
	return NodeRegistries.Add(Class, NewRegistry);
}

void UHeartNodeRegistrySubsystem::AddRegistrar(UGraphNodeRegistrar* Registrar)
{
	for (auto&& GraphClass : Registrar->RegisterWith)
	{
		GetRegistry(GraphClass)->AddRegistrar(Registrar);
	}
}

void UHeartNodeRegistrySubsystem::RemoveRegistrar(UGraphNodeRegistrar* Registrar)
{
	for (auto&& GraphClass : Registrar->RegisterWith)
	{
		GetRegistry(GraphClass)->RemoveRegistrar(Registrar);
	}
}
