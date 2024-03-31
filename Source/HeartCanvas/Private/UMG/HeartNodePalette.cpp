// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartNodePalette.h"

#include "UMG/HeartGraphCanvas.h" // For log category
#include "Components/PanelWidget.h"
#include "GraphRegistry/HeartRegistryQuery.h"
#include "Model/HeartGraphNode.h"
#include "General/HeartContextObject.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartNodePalette)

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

UHeartNodePalette::UHeartNodePalette()
{
	Query = CreateDefaultSubobject<UHeartRegistryQuery>(TEXT("Query"));
}

bool UHeartNodePalette::Initialize()
{
	auto Super = Super::Initialize();

	BindingContainer.SetupLinker(this);

	return Super;
}

UHeartInputLinkerBase* UHeartNodePalette::ResolveLinker_Implementation() const
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

void UHeartNodePalette::Display(const TArray<FHeartNodeArchetype>& Classes)
{
	if (!ensure(IsValid(PalettePanel)))
	{
		return;
	}

	for (auto&& ClassPair : Classes)
	{
		if (!ensure(ClassPair.Source.IsValid() && IsValid(ClassPair.GraphNode)))
		{
			continue;
		}

		if (!ShouldDisplayNode(ClassPair))
		{
			continue;
		}

		if (IsValid(CategoryClass))
		{
			FText Category = ClassPair.GraphNode->GetDefaultObject<UHeartGraphNode>()->GetDefaultNodeCategory(ClassPair.Source);

			if (UHeartNodePaletteCategory* CategoryWidget = FindOrCreateCategory(Category))
			{
				CategoryWidget->AddNode(ClassPair.Source);
				continue;
			}
		}

		if (auto&& NewPaletteEntry = CreateNodeWidgetFromFactory(ClassPair.Source))
		{
			PalettePanel->AddChild(NewPaletteEntry);
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
	// @todo we have to const_cast here unfortunately, because Blueprint doesn't understand const
	UObject* Obj = const_cast<UObject*>(NodeSource.As<UObject>());

	if (auto&& WidgetClass = WidgetFactory.GetWidgetClass(NodeSource.As<UObject>()))
	{
		auto&& NewNodeWidget = CreateWidget(this, WidgetClass);

		// Try to give the node widget the node class as a Context object. This is optional, technically, but
		// highly suggested.
		if (NewNodeWidget->Implements<UHeartContextObject>())
		{
			IHeartContextObject::Execute_SetContextObject(NewNodeWidget, Obj);
		}

		return NewNodeWidget;
	}

	UE_LOG(LogHeartGraphCanvas, Warning, TEXT("WidgetClass not found in WidgetFactory for '%s'. It will not be displayed!"), *Obj->GetName())
	return nullptr;
}

void UHeartNodePalette::RefreshPalette()
{
	Reset();
	TArray<FHeartNodeArchetype> NodeClasses;
	Query->Run(DisplayedRegistryGraph, NodeClasses);
	Display(NodeClasses);
}

bool UHeartNodePalette::ShouldDisplayNode_Implementation(const FHeartNodeArchetype Archetype)
{
	if (Archetype.Source.ThisClass()->HasAnyClassFlags(CLASS_Abstract)) return false;
	if (Archetype.GraphNode->HasAnyClassFlags(CLASS_Abstract)) return false;

	return GetDefault<UHeartGraphNode>(Archetype.GraphNode)->CanCreate();
}