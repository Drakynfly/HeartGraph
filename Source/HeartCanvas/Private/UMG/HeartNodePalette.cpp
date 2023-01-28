// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartNodePalette.h"

#include "UMG/HeartGraphCanvas.h" // For log category
#include "Components/PanelWidget.h"
#include "GraphRegistry/HeartNodeRegistrySubsystem.h"
#include "Model/HeartGraphNode.h"
#include "UI/HeartUMGContextObject.h"

UHeartNodePalette* UHeartNodePaletteCategory::GetPalette() const
{
	return GetTypedOuter<UHeartNodePalette>();
}

UWidget* UHeartNodePaletteCategory::MakeWidgetForNode_Implementation(UClass* NodeClass)
{
	auto&& Palette = GetPalette();
	if (!IsValid(Palette)) return nullptr;
	return Palette->CreateNodeWidgetFromFactory(NodeClass);
}

bool UHeartNodePalette::Initialize()
{
	auto Super = Super::Initialize();

	BindingContainer.SetupLinker(this);

	return Super;
}

UHeartWidgetInputLinker* UHeartNodePalette::ResolveLinker_Implementation() const
{
	return BindingContainer.GetLinker();
}

void UHeartNodePalette::Reset()
{
	if (!ensure(IsValid(PalettePanel)))
	{
		return;
	}

	for (auto&& Category : Categories)
	{
		Category.Value->ClearChildren();
	}

	PalettePanel->ClearChildren();

	OnReset();
}

void UHeartNodePalette::Display(const TMap<UClass*, TSubclassOf<UHeartGraphNode>>& Classes)
{
	if (!ensure(IsValid(PalettePanel)))
	{
		return;
	}

	for (auto&& ClassPair : Classes)
	{
		UClass* NodeClass = ClassPair.Key;
		TSubclassOf<UHeartGraphNode> GraphNode = ClassPair.Value;

		if (!ensure(IsValid(NodeClass) || !ensure(IsValid(GraphNode))))
		{
			continue;
		}

		if (!ShouldDisplayNode(NodeClass, GraphNode))
		{
			continue;
		}

		FText Category = GraphNode->GetDefaultObject<UHeartGraphNode>()->GetDefaultNodeCategory(NodeClass);

		if (UHeartNodePaletteCategory* CategoryWidget = FindOrCreateCategory(Category))
		{
			CategoryWidget->AddNode(NodeClass);
		}
		else
		{
			if (auto&& NewPaletteEntry = CreateNodeWidgetFromFactory(NodeClass))
			{
				PalettePanel->AddChild(NewPaletteEntry);
			}
		}
	}

	OnDisplay();
}

UHeartNodePaletteCategory* UHeartNodePalette::FindOrCreateCategory(const FText& Category)
{
	if (Category.IsEmpty())
	{
		return nullptr;
	}

	const FString CategoryStr = Category.ToString();

	if (auto&& ExistingCategory = Categories.Find(CategoryStr))
	{
		if (!ExistingCategory->Get()->GetParent())
		{
			PalettePanel->AddChild(*ExistingCategory);
		}

		return *ExistingCategory;
	}

	if (IsValid(CategoryClass))
	{
		UHeartNodePaletteCategory* NewCategory = CreateWidget<UHeartNodePaletteCategory>(this, CategoryClass);
		NewCategory->SetLabel(Category);
		Categories.Add(CategoryStr, NewCategory);
		PalettePanel->AddChild(NewCategory);
		return NewCategory;
	}

	return nullptr;
}

UUserWidget* UHeartNodePalette::CreateNodeWidgetFromFactory(UClass* NodeClass)
{
	if (auto&& WidgetClass = WidgetFactory.GetWidgetClass(NodeClass))
	{
		auto&& NewNodeWidget = CreateWidget(this, WidgetClass);

		// Try to give the node widget the node class as a Context object. This is optional, technically, but
		// highly suggested.
		if (NewNodeWidget->Implements<UHeartUMGContextObject>())
		{
			IHeartUMGContextObject::Execute_SetContextObject(NewNodeWidget, NodeClass);
		}

		return NewNodeWidget;
	}

	UE_LOG(LogHeartGraphCanvas, Warning, TEXT("WidgetClass not found in WidgetFactory for '%s'. It will not be displayed!"), *NodeClass->GetName())
	return nullptr;
}

void UHeartNodePalette::RefreshPalette()
{
	Reset();

	auto&& NodeRegistrySubsystem = GEngine->GetEngineSubsystem<UHeartNodeRegistrySubsystem>();

	if (auto&& Registry = NodeRegistrySubsystem->GetRegistry(DisplayedRegistryGraph))
	{
		TMap<UClass*, TSubclassOf<UHeartGraphNode>> NodeClasses;

		if (Filter.IsBound())
		{
			Registry->GetFilteredNodeClassesWithGraphClass(Filter, NodeClasses);
		}
		else
		{
			Registry->GetNodeClassesWithGraphClass(NodeClasses);
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

bool UHeartNodePalette::ShouldDisplayNode_Implementation(const UClass* NodeClass,
                                                         const TSubclassOf<UHeartGraphNode> GraphNodeClass)
{
	if (NodeClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated)) return false;
	if (GraphNodeClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated)) return false;

	return GetDefault<UHeartGraphNode>(GraphNodeClass)->CanCreate();
}
