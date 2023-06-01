// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartNodeSource.generated.h"

/**
 * A node source is a UObject from which a Heart node can be made. These are usually UClasses, but not always.
 */
USTRUCT(BlueprintType, meta = (DisableSplitPin))
struct FHeartNodeSource
{
	GENERATED_BODY()

	FHeartNodeSource() {}

	explicit FHeartNodeSource(const UObject* SourceObject)
	{
		NodeObj = SourceObject;
	}

	template <typename T>
	T* As() const { return Cast<T>(NodeObj); }

	bool IsValid() const {return ::IsValid(NodeObj); }

	bool IsAOrClassOf(const UClass* Class) const
	{
		if (const UClass* AsClass = As<UClass>())
		{
			return AsClass->IsChildOf(Class);
		}
		return NodeObj.IsA(Class);
	}

	// Returns either the NodeObj if its a UClass, or the Class of the NodeObj otherwise
	UClass* ThisClass() const
	{
		if (UClass* AsClass = As<UClass>())
		{
			return AsClass;
		}
		return NodeObj->GetClass();
	}

	// Returns either the SuperClass if its a UClass, or the Class of the NodeObj otherwise
	UClass* NextClass() const
	{
		if (UClass* AsClass = As<UClass>())
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
	friend bool operator==(const FHeartNodeSource Lhs, const FHeartNodeSource RHS)
	{
		return Lhs.NodeObj == RHS.NodeObj;
	}

	friend bool operator!=(const FHeartNodeSource Lhs, const FHeartNodeSource RHS)
	{
		return !(Lhs == RHS);
	}

	friend uint32 GetTypeHash(const FHeartNodeSource SourceObject)
	{
		return GetTypeHash(SourceObject.NodeObj);
	}
};