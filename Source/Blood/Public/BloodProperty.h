// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning

#pragma once

#include "BloodData.h"
#include "BloodFProperty.h"
#include "EdGraph/EdGraphPin.h"

#include "BloodProperty.generated.h"

USTRUCT()
struct BLOOD_API FBloodProperty
{
	GENERATED_BODY()

	FBloodProperty() = default;

	UPROPERTY(EditDefaultsOnly, Category = "BloodProperty")
	FName PropName = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category = "BloodProperty")
	FText ToolTip;

	UPROPERTY(EditDefaultsOnly, Category = "BloodProperty")
	EPinContainerType ContainerType = EPinContainerType::None;

	// The UClass or UStruct that this type represents
	UPROPERTY(EditDefaultsOnly, Category = "BloodProperty")
	TObjectPtr<const UField> FieldType = nullptr;

	// Type of the Value when this is a map
	UPROPERTY(EditDefaultsOnly, Category = "BloodProperty", meta = (EditCondition = "ContainerType == EPinContainerType::Map", EditConditionHides))
	TObjectPtr<const UField> MapValueFieldType = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "BloodProperty")
	bool Const = false;

#if WITH_EDITOR
	// @todo move this somewhere else. I think only Plato is using it anyway
	FEdGraphPinType ToEdGraphPin() const
	{
		return FEdGraphPinType("struct", NAME_None, ConstCast(FieldType), ContainerType, false, FEdGraphTerminalType());
	}
#endif

	FORCEINLINE bool IsValid() const
	{
		return !PropName.IsNone() && ::IsValid(FieldType);
	}

	FORCEINLINE bool operator==(const FBloodProperty& Other) const
	{
		return PropName == Other.PropName;
	}

	FORCEINLINE bool operator!=(const FBloodProperty& Other) const
	{
		return PropName != Other.PropName;
	}

	FORCEINLINE bool operator==(FName Other) const
	{
		return PropName == Other;
	}

	FORCEINLINE bool operator!=(FName Other) const
	{
		return PropName != Other;
	}

	friend uint32 GetTypeHash(const FBloodProperty& BloodType)
	{
		return GetTypeHash(BloodType.PropName);
	}
};

namespace Blood
{
	// Enum type for containers. Used when a *Value* is needed.
	enum EContainerType
	{
		Single,
		Array,
		Set,
		Map
	};

	// Struct types for containers. Used when a *Typename* is needed.
	struct TSingle { static constexpr EContainerType Type = Single; };
	struct TArray { static constexpr EContainerType Type = Array; };
	struct TSet { static constexpr EContainerType Type = Set; };
	struct TMap { static constexpr EContainerType Type = Map; };

#if WITH_EDITOR
	// Pizza class for creating FBloodProperties
	struct FBloodPropertyFactory
	{
		// Constructor that has the minimum required parameters to create an FBloodProperty
		FBloodPropertyFactory(FName InPropName, UStruct* Struct, EContainerType ContainerType)
		{
			Type.PropName = InPropName;

			if (IsValid(Struct))
			{
				Type.FieldType = Struct;
			}
			else
			{
				Type.FieldType = FBloodWildcard::StaticStruct();
			}

			Type.ContainerType = static_cast<EPinContainerType>(ContainerType);
		}

		// Return operator to assign this factory to a Prop variable.
		operator::FBloodProperty() const
		{
			return Type;
		}

		// Pizza function for setting the tooltip
		FBloodPropertyFactory& ToolTip(const FText& Value)
		{
			Type.ToolTip = Value;
			return *this;
		}

		// Pizza function for marking the type as Const.
		FBloodPropertyFactory& Const()
		{
			Type.Const = true;
			return *this;
		}

	private:
		FBloodProperty Type;
	};

	template <typename TBloodData, typename Container = TSingle>
	struct CreatePin : FBloodPropertyFactory
	{
		CreatePin(const FName Name)
			: FBloodPropertyFactory(Name, TDataConverter<TBloodData>::Type(), Container::Type)
		{}
	};

	template <typename Container>
	struct CreatePin<void, Container> : FBloodPropertyFactory
	{
		CreatePin(const FName Name, UStruct* Struct = nullptr)
		  : FBloodPropertyFactory(Name, Struct, Container::Type)
		{}
	};

	// Specialization for TObjectPtr
	template <typename T, typename Container>
	struct CreatePin<TObjectPtr<T>, Container> : FBloodPropertyFactory
	{
		CreatePin(const FName Name)
		  : FBloodPropertyFactory(Name, T::StaticClass(), Container::Type) {}
	};

	// Specialization for TSubclassOf
	template <typename T, typename Container>
	struct CreatePin<TSubclassOf<T>, Container> : FBloodPropertyFactory
	{
		CreatePin(const FName Name)
		  : FBloodPropertyFactory(Name, T::StaticClass(), Container::Type) {}
	};

	// Specialization for TSoftObjectPtr
	template <typename T, typename Container>
	struct CreatePin<TSoftObjectPtr<T>, Container> : FBloodPropertyFactory
	{
		CreatePin(const FName Name)
		  : FBloodPropertyFactory(Name, T::StaticClass(), Container::Type) {}
	};

	// Specialization for TSoftClassPtr
	template <typename T, typename Container>
	struct CreatePin<TSoftClassPtr<T>, Container> : FBloodPropertyFactory
	{
		CreatePin(const FName Name)
		  : FBloodPropertyFactory(Name, T::StaticClass(), Container::Type) {}
	};
#endif

	FORCEINLINE_DEBUGGABLE FBloodProperty FPropertyToBloodProperty(const FProperty* Prop)
	{
		FBloodProperty OutProp;

		OutProp.PropName = Prop->GetFName();

#if WITH_EDITOR
		OutProp.ToolTip = Prop->GetToolTipText();
#endif

		if (auto&& ArrayProp = CastField<FArrayProperty>(Prop))
		{
			OutProp.ContainerType = EPinContainerType::Array;
			OutProp.FieldType = GetFPropertyFieldType(ArrayProp->Inner);
		}
		else if (auto&& SetProp = CastField<FSetProperty>(Prop))
		{
			OutProp.ContainerType = EPinContainerType::Set;
			OutProp.FieldType =  GetFPropertyFieldType(SetProp->ElementProp);
		}
		else if (auto&& MapProp = CastField<FMapProperty>(Prop))
		{
			OutProp.ContainerType = EPinContainerType::Map;
			OutProp.FieldType = GetFPropertyFieldType(MapProp->KeyProp);
			OutProp.MapValueFieldType = GetFPropertyFieldType(MapProp->ValueProp);
		}
		else
		{
			OutProp.ContainerType = EPinContainerType::None;
			OutProp.FieldType = GetFPropertyFieldType(Prop);
		}

		return OutProp;
	}
}