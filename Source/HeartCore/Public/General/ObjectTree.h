﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ObjectTree.generated.h"

namespace Heart
{
	struct FObjectNode : TSharedFromThis<FObjectNode>
	{
		using FKeyType = TWeakObjectPtr<const UClass>;

		TSharedPtr<FObjectNode> FindChildNode(FKeyType Class) const;
		TSharedRef<FObjectNode> FindOrAddChildNode(FKeyType Class);

		TMap<FKeyType, TSharedRef<FObjectNode>> Children;
		TArray<TWeakObjectPtr<UObject>> ObjectList;
	};

	/**
	 * An object tree is composed of nodes that represent UClasses, and stores a list of objects of that class.
	 * Only the objects added by calling AddObject will exist in the tree.
	 * This type can be used to build custom asset filters that only display from a list of manually authored objects.
	 */
	struct HEARTCORE_API FObjectTree : FObjectNode
	{
		static TSharedRef<FObjectTree> MakeTree();

		void AddObject(UObject* Obj);
		void RemoveObject(const UObject* Obj);
		void RemoveNode(const UClass* Class);

		TArray<TWeakObjectPtr<UObject>> GetObjects(TWeakObjectPtr<const UClass> Class) const;
	};
}


UCLASS(BlueprintType)
class HEARTCORE_API UHeartObjectTree : public UObject
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;

	UFUNCTION(BlueprintCallable, Category = "Heart|ObjectTree")
	void AddObjectToTree(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Heart|ObjectTree", meta = (DeterminesOutputType = "Class"))
	TArray<UObject*> GetObjectsInTree(const UClass* Class) const;

private:
	TSharedPtr<Heart::FObjectTree> Tree = nullptr;
};