// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartNodePalette.h"

#include "Components/PanelWidget.h"
#include "GraphRegistry/HeartNodeRegistrySubsystem.h"
#include "UI/HeartUMGContextObject.h"

bool UHeartNodePalette::Initialize()
{
	auto Super = Super::Initialize();

	BindingContainer.SetupLinker(this);

	return Super;
}

void UHeartNodePalette::Reset()
{
	if (!ensure(IsValid(PalettePanel)))
	{
		return;
	}

	PalettePanel->ClearChildren();

	OnReset();
}

void UHeartNodePalette::Display(const TArray<UClass*>& Classes)
{
	if (!ensure(IsValid(PalettePanel)))
	{
		return;
	}

	for (auto&& Class : Classes)
	{
		if (ensure(IsValid(Class)))
		{
			if (auto&& WidgetClass = Rules.GetWidgetClass(Class))
			{
				auto&& NewPaletteEntry = CreateWidget(this, WidgetClass);
				if (NewPaletteEntry->Implements<UHeartUMGContextObject>())
				{
					IHeartUMGContextObject::Execute_SetContextObject(NewPaletteEntry, Class);
				}
				PalettePanel->AddChild(NewPaletteEntry);
			}
		}
	}

	OnDisplay();
}

UHeartWidgetInputLinker* UHeartNodePalette::ResolveLinker_Implementation() const
{
	return BindingContainer.GetLinker();
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
