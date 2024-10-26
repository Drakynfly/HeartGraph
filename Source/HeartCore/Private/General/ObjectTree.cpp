// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "General/ObjectTree.h"

namespace Heart
{
	TSharedPtr<FObjectNode> FObjectNode::FindChildNode(const TWeakObjectPtr<const UClass> Class) const
	{
		if (auto&& Child = Children.Find(Class))
		{
			return *Child;
		}
		return nullptr;
	}

	TSharedRef<FObjectNode> FObjectNode::FindOrAddChildNode(const FKeyType Class)
	{
		if (auto&& Child = Children.Find(Class))
		{
			return *Child;
		}

		return Children.Add(Class, MakeShared<FObjectNode>());
	}

	TSharedRef<FObjectTree> FObjectTree::MakeTree()
	{
		return MakeShared<FObjectTree>();
	}

	void FObjectTree::AddObject(UObject* Obj)
	{
		if (!IsValid(Obj)) return;

		// Make list of classes.
		TArray<const UClass*> Classes;
		for (const UClass* Class = Obj->GetClass(); Class && Class != UObject::StaticClass(); Class = Class->GetSuperClass())
		{
			Classes.Add(Class);
		}

		TSharedRef<FObjectNode> Node = SharedThis(this);
		for (int32 i = Classes.Num() - 1; i >= 0; --i)
		{
			Node = Node->FindOrAddChildNode(Classes[i]);
		}

		Node->ObjectList.AddUnique(Obj);
	}

	void FObjectTree::RemoveObject(const UObject* Obj)
	{
		if (!IsValid(Obj)) return;

		// Make list of classes.
		TArray<const UClass*> Classes;
		for (const UClass* Class = Obj->GetClass(); Class && Class != UObject::StaticClass(); Class = Class->GetSuperClass())
		{
			Classes.Add(Class);
		}

		TSharedPtr<FObjectNode> Node = SharedThis(this);
		for (int32 i = Classes.Num() - 1; i >= 0; --i)
		{
			Node = Node->FindChildNode(Classes[i]);
			if (!Node.IsValid())
			{
				return;
			}
		}

		if (Node.IsValid())
		{
			Node->ObjectList.Remove(ConstCast(ObjectPtrWrap(Obj)));
		}
	}

	void FObjectTree::RemoveNode(const UClass* Class)
	{
		if (!IsValid(Class)) return;

		// Make list of classes.
		TArray<const UClass*> Classes;
		for ( ; Class && Class != UObject::StaticClass(); Class = Class->GetSuperClass())
		{
			Classes.Add(Class);
		}

		TSharedPtr<FObjectNode> Parent = nullptr;
		TSharedPtr<FObjectNode> Node = SharedThis(this);
		for (int32 i = Classes.Num() - 1; i >= 0; --i)
		{
			Parent = Node;
			Node = Node->FindChildNode(Classes[i]);
			if (!Node.IsValid())
			{
				return;
			}
		}

		// Loop was exited without returning, this parent/node is valid, so remove it.
		Parent->Children.Remove(Classes[0]);
	}

	TArray<TWeakObjectPtr<UObject>> FObjectTree::GetObjects(const TWeakObjectPtr<const UClass> Class) const
	{
		TArray<TSharedRef<FObjectNode>> Nodes;
		if (Class == UObject::StaticClass())
		{
			Children.GenerateValueArray(Nodes);
		}
		else if (auto&& Child = FindChildNode(Class))
		{
			Nodes.Add(Child.ToSharedRef());
		}

		TArray<TWeakObjectPtr<UObject>> OutObjects;

		while (!Nodes.IsEmpty())
		{
			auto&& Node = Nodes.Pop(EAllowShrinking::No);
			OutObjects.Append(Node->ObjectList);

			for (auto&& Child : Node->Children)
			{
				Nodes.Add(Child.Value);
			}
		}
		return OutObjects;
	}
}

void UHeartObjectTree::PostInitProperties()
{
	Super::PostInitProperties();
	Tree = MakeShared<Heart::FObjectTree>();
}

void UHeartObjectTree::AddObjectToTree(UObject* Object)
{
	if (Tree.IsValid())
	{
		Tree->AddObject(Object);
	}
}

TArray<UObject*> UHeartObjectTree::GetObjectsInTree(const UClass* Class) const
{
	if (!Tree.IsValid()) return {};

	TArray<TWeakObjectPtr<UObject>> Options = Tree->GetObjects(Class);

	TArray<UObject*> SortedOptions;
	SortedOptions.Reserve(Options.Num());
	for (auto&& Object : Options)
	{
		if (Object.IsValid())
		{
			SortedOptions.Add(Object.Get());
		}
	}
	Algo::Sort(SortedOptions,
		[](const UObject* A, const UObject* B)
		{
			return A->GetName() < B->GetName();
		});

	return SortedOptions;
}