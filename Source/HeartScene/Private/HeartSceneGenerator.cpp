// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartSceneGenerator.h"

#include "HeartSceneModule.h"
#include "HeartSceneNode.h"
#include "Engine/Engine.h"
#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "ModelView/HeartGraphSchema.h"
#include "Location/HeartNodeLocationModifier.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSceneGenerator)

UHeartSceneGenerator::UHeartSceneGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
	UseWorldSpaceInModifiers = false;

	LocationModifiers = CreateDefaultSubobject<UHeartNodeLocationModifierStack>("LocationModifiers");
}

UHeartGraph* UHeartSceneGenerator::GetHeartGraph_Implementation() const
{
	return Graph;
}

FVector2D UHeartSceneGenerator::GetNodeLocation(const FHeartNodeGuid& Node) const
{
	if (!ensure(IsValid(LocationModifiers))) return FVector2D();

	if (const TObjectPtr<UHeartSceneNode>* SceneNode = SceneNodes.Find(Node))
	{
		if (IsValid(*SceneNode))
		{
			return LocationModifiers->ProxyToLocation(FVector2D(UseWorldSpaceInModifiers ? (*SceneNode)->GetComponentLocation() : (*SceneNode)->GetRelativeLocation()));
		}
	}

	return FVector2D();
}

void UHeartSceneGenerator::SetNodeLocation(const FHeartNodeGuid& Node, const FVector2D& Location, const bool InProgressMove)
{
	if (!ensure(IsValid(LocationModifiers))) return;

	if (const TObjectPtr<UHeartSceneNode>* SceneNode = SceneNodes.Find(Node))
	{
		if (IsValid(*SceneNode))
		{
			FVector Proxy(Location, 0.0);

			if (UseWorldSpaceInModifiers)
			{
				Proxy.Z = (*SceneNode)->GetComponentLocation().Z;
			}
			else
			{
				Proxy.Z = (*SceneNode)->GetRelativeLocation().Z;
			}

			Proxy = LocationModifiers->LocationToProxy3D(Proxy);

			if (UseWorldSpaceInModifiers)
			{
				(*SceneNode)->SetWorldLocation(Proxy);
			}
			else
			{
				(*SceneNode)->SetRelativeLocation(Proxy);
			}
		}
	}
}

FVector UHeartSceneGenerator::GetNodeLocation3D(const FHeartNodeGuid& Node) const
{
	if (!ensure(IsValid(LocationModifiers))) return FVector();

	if (const TObjectPtr<UHeartSceneNode>* SceneNode = SceneNodes.Find(Node))
	{
		if (IsValid(*SceneNode))
		{
			return LocationModifiers->ProxyToLocation3D(UseWorldSpaceInModifiers ? (*SceneNode)->GetComponentLocation() : (*SceneNode)->GetRelativeLocation());
		}
	}

	return FVector();
}

void UHeartSceneGenerator::SetNodeLocation3D(const FHeartNodeGuid& Node, const FVector& Location, const bool InProgressMove)
{
	if (!ensure(IsValid(LocationModifiers))) return;

	if (const TObjectPtr<UHeartSceneNode>* SceneNode = SceneNodes.Find(Node))
	{
		if (IsValid(*SceneNode))
		{
			const FVector Proxy = LocationModifiers->LocationToProxy3D(Location);

            if (UseWorldSpaceInModifiers)
            {
            	(*SceneNode)->SetWorldLocation(Proxy);
            }
            else
            {
            	(*SceneNode)->SetRelativeLocation(Proxy);
            }
		}
	}
}

void UHeartSceneGenerator::SetDisplayedGraph(UHeartGraph* NewGraph)
{
	if (IsValid(Graph))
	{
		OnReset();
	}

	Graph = NewGraph;
}

void UHeartSceneGenerator::Regenerate()
{
	OnReset();
	Generate();
}

UHeartSceneNode* UHeartSceneGenerator::GetSceneNode(const FHeartNodeGuid& NodeGuid) const
{
	if (auto&& Node = SceneNodes.Find(NodeGuid))
	{
		return *Node;
	}
	return nullptr;
}

void UHeartSceneGenerator::OnReset_Implementation()
{
}

void UHeartSceneGenerator::Generate_Implementation()
{
}

TSubclassOf<UHeartSceneNode> UHeartSceneGenerator::GetVisualClassForNode(const UHeartGraphNode* GraphNode) const
{
	auto&& RegistrySubsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>();

	if (!IsValid(RegistrySubsystem))
	{
		UE_LOG(LogHeartGraphScene, Error,
			TEXT("Registry Subsystem not found! Make sure to enable `CreateRuntimeRegistrySubsystem` in project settings to access the subsystem!\n"
					"This error occured in UHeartSceneGenerator::GetVisualClassForNode. You can override this function to not use the registry subsystem if `CreateRuntimeRegistrySubsystem` is disabled on purpose!"))
		return nullptr;
	}

	return RegistrySubsystem->GetNodeRegistryForGraph(Graph)
								->GetVisualizerClassForGraphNode<UHeartSceneNode>(GraphNode->GetClass());
}

UHeartSceneNode* UHeartSceneGenerator::AddNodeToDisplay(UHeartGraphNode* GraphNode)
{
	// This function is only used internally, so Node should *always* be validated prior to this point.
	check(GraphNode);

	if (const TSubclassOf<UHeartSceneNode> VisualizerClass = GetVisualClassForNode(GraphNode))
	{
		auto&& SceneNode = NewObject<UHeartSceneNode>(GetOwner(), VisualizerClass);
		check(SceneNode);

		SceneNode->Generator = this;
		SceneNode->GraphNode = GraphNode->GetGuid();

		SceneNodes.Add(GraphNode->GetGuid(), SceneNode);

		SceneNode->RegisterComponent();

		SceneNode->NativeOnCreated();

		return SceneNode;
	}
	else
	{
		UE_LOG(LogHeartGraphScene, Warning, TEXT("Unable to determine Visual Class. Node '%s' will not be displayed"), *GraphNode->GetName())
		return nullptr;
	}
}