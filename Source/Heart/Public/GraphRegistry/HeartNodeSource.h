// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "HeartNodeSource.generated.h"

/**
 * A node source is a UObject from which a Heart node can be made. These are usually UClasses, but not always.
 */
USTRUCT(BlueprintType, meta = (DisableSplitPin))
struct FHeartNodeSource
{
	GENERATED_BODY()

	FHeartNodeSource() = default;

	explicit FHeartNodeSource(const UObject* SourceObject)
	{
		NodeObj = SourceObject;
	}

	template <typename T>
	const T* As() const { return Cast<T>(NodeObj); }

	bool IsValid() const { return ::IsValid(NodeObj); }

	bool IsAOrClassOf(const UClass* Class) const
	{
		if (const UClass* AsClass = As<UClass>())
		{
			return AsClass->IsChildOf(Class);
		}
		return NodeObj.IsA(Class);
	}

	// Returns either the NodeObj if it's a UClass, or the Class of the NodeObj otherwise
	const UClass* ThisClass() const
	{
		if (const UClass* AsClass = As<UClass>())
		{
			return AsClass;
		}
		return NodeObj->GetClass();
	}

	// Returns either the SuperClass if it's a UClass, or the Class of the NodeObj otherwise
	const UClass* NextClass() const
	{
		if (const UClass* AsClass = As<UClass>())
		{
			return AsClass->GetSuperClass();
		}
		return NodeObj->GetClass();
	}

	const UObject* GetDefaultObject() const
	{
		if (const UClass* AsClass = As<UClass>())
		{
			return AsClass->GetDefaultObject();
		}
		if (NodeObj->HasAnyFlags(RF_ClassDefaultObject))
		{
			return NodeObj;
		}
		return NodeObj->GetClass()->GetDefaultObject();
	}

private:
	UPROPERTY()
	TObjectPtr<const UObject> NodeObj;

public:
	friend bool operator==(const FHeartNodeSource Lhs, const FHeartNodeSource Rhs)
	{
		return Lhs.NodeObj == Rhs.NodeObj;
	}

	friend bool operator!=(const FHeartNodeSource Lhs, const FHeartNodeSource Rhs)
	{
		return !(Lhs == Rhs);
	}

	FORCEINLINE friend uint32 GetTypeHash(const FHeartNodeSource SourceObject)
	{
		return GetTypeHash(SourceObject.NodeObj);
	}

	void AddStructReferencedObjects(FReferenceCollector& Collector)
	{
		Collector.AddStableReference(&ConstCast(NodeObj));
	}
};

template<>
struct TStructOpsTypeTraits<FHeartNodeSource> : public TStructOpsTypeTraitsBase2<FHeartNodeSource>
{
	enum
	{
		WithAddStructReferencedObjects = true,
	};
};

class UHeartGraphNode;

/**
 * A Node Archetype is the minimum data required to create a new node instance in a graph.
 */
USTRUCT(BlueprintType)
struct FHeartNodeArchetype
{
	GENERATED_BODY()

	FHeartNodeArchetype() = default;
	FHeartNodeArchetype(const TSubclassOf<UHeartGraphNode>& GraphNode, const FHeartNodeSource& Source)
	  : GraphNode(GraphNode), Source(Source) {}

	// The class to instance a GraphNode with
	UPROPERTY()
	TSubclassOf<UHeartGraphNode> GraphNode;

	// The UObject to create a NodeObject from
	UPROPERTY()
	FHeartNodeSource Source;
};