// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartNodePalette.h"

#include "UMG/HeartGraphCanvas.h" // For log category
#include "Components/PanelWidget.h"
#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#include "Model/HeartGraphNode.h"
#include "UI/HeartUMGContextObject.h"

UHeartNodePalette* UHeartNodePaletteCategory::GetPalette() const
{
	return GetTypedOuter<UHeartNodePalette>();
}

UWidget* UHeartNodePaletteCategory::MakeWidgetForNode_Implementation(const FHeartNodeSource NodeSource)
{
	auto&& Palette = GetPalette();
	if (!IsValid(Palette)) return nullptr;
	return Palette->CreateNodeWidgetFromFactory(NodeSource);
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

void UHeartNodePalette::Display(const TMap<FHeartNodeSource, TSubclassOf<UHeartGraphNode>>& Classes)
{
	if (!ensure(IsValid(PalettePanel)))
	{
		return;
	}

	for (auto&& ClassPair : Classes)
	{
		FHeartNodeSource NodeSource = ClassPair.Key;
		TSubclassOf<UHeartGraphNode> GraphNode = ClassPair.Value;

		if (!ensure(NodeSource.IsValid() && IsValid(GraphNode)))
		{
			continue;
		}

		if (!ShouldDisplayNode(NodeSource, GraphNode))
		{
			continue;
		}

		FText Category = GraphNode->GetDefaultObject<UHeartGraphNode>()->GetDefaultNodeCategory(NodeSource);

		if (UHeartNodePaletteCategory* CategoryWidget = FindOrCreateCategory(Category))
		{
			CategoryWidget->AddNode(NodeSource);
		}
		else
		{
			if (auto&& NewPaletteEntry = CreateNodeWidgetFromFactory(NodeSource))
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

UUserWidget* UHeartNodePalette::CreateNodeWidgetFromFactory(const FHeartNodeSource NodeSource)
{
	UObject* Obj = NodeSource.As<UObject>();

	if (auto&& WidgetClass = WidgetFactory.GetWidgetClass(NodeSource.As<UObject>()))
	{
		auto&& NewNodeWidget = CreateWidget(this, WidgetClass);

		// Try to give the node widget the node class as a Context object. This is optional, technically, but
		// highly suggested.
		if (NewNodeWidget->Implements<UHeartUMGContextObject>())
		{
			IHeartUMGContextObject::Execute_SetContextObject(NewNodeWidget, Obj);
		}

		return NewNodeWidget;
	}

	UE_LOG(LogHeartGraphCanvas, Warning, TEXT("WidgetClass not found in WidgetFactory for '%s'. It will not be displayed!"), *Obj->GetName())
	return nullptr;
}

void UHeartNodePalette::RefreshPalette()
{
	Reset();

	auto&& NodeRegistrySubsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>();

	if (auto&& Registry = NodeRegistrySubsystem->GetRegistry(DisplayedRegistryGraph))
	{
		TMap<FHeartNodeSource, TSubclassOf<UHeartGraphNode>> NodeClasses;

		Registry->QueryGraphAndNodeClasses(Query, NodeClasses);

		Display(NodeClasses);
	}
}

void UHeartNodePalette::SetFilter(const FNodeSourceFilter& NewFilter, const bool bRefreshPalette)
{
	Query.Filter = NewFilter;

	if (bRefreshPalette)
	{
		RefreshPalette();
	}
}

void UHeartNodePalette::ClearFilter(const bool bRefreshPalette)
{
	Query.Filter.Unbind();

	if (bRefreshPalette)
	{
		RefreshPalette();
	}
}

bool UHeartNodePalette::ShouldDisplayNode_Implementation(const FHeartNodeSource NodeClass,
                                                         const TSubclassOf<UHeartGraphNode> GraphNodeClass)
{
	if (NodeClass.ThisClass()->HasAnyClassFlags(CLASS_Abstract)) return false;
	if (GraphNodeClass->HasAnyClassFlags(CLASS_Abstract)) return false;

	return GetDefault<UHeartGraphNode>(GraphNodeClass)->CanCreate();
}
