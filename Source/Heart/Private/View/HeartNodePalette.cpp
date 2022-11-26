// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "View/HeartNodePalette.h"
#include "GraphRegistry/HeartNodeRegistrySubsystem.h"

void UHeartNodePalette::Reset()
{
	BP_Reset();
}

void UHeartNodePalette::Display(const TArray<UClass*>& Classes)
{
	BP_Display(Classes);
}

void UHeartNodePalette::RefreshPalette()
{
	Reset();

	auto&& NodeRegistrySubsystem = GEngine->GetEngineSubsystem<UHeartNodeRegistrySubsystem>();

	if (auto&& Registry = NodeRegistrySubsystem->GetRegistry(DisplayedRegistryGraph))
	{
		TArray<UClass*> NodeClasses;

		if (Filter.IsBound())
		{
			Registry->GetFilteredNodeClasses(Filter, NodeClasses);
		}
		else
		{
			Registry->GetNodeClasses(NodeClasses);
		}

		Display(NodeClasses);
	}
}

void UHeartNodePalette::SetFilter(const FNodeClassFilter& NewFilter, const bool bRefreshPalette)
{
	Filter = NewFilter;

	if (bRefreshPalette)
	{
		RefreshPalette();
	}
}

void UHeartNodePalette::ClearFilter(const bool bRefreshPalette)
{
	Filter.Unbind();

	if (bRefreshPalette)
	{
		RefreshPalette();
	}
}
