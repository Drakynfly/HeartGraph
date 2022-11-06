// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once


#include "UObject/Object.h"
#include "InstancedStruct.h"
#include "Components/Widget.h"
#include "HeartDragDropOperation.h"
#include "HeartWidgetInputBindingContainer.generated.h"

DECLARE_DELEGATE_RetVal_OneParam(
	bool, FHeartWidgetInputCondition, UWidget* /** Widget */);

DECLARE_DELEGATE_RetVal_TwoParams(
	FReply, FHeartWidgetLinkedInput, UWidget* /** Widget */, const struct FHeartInputActivation& /** Activation */);

// @todo add more data from the event to operator== to support modifier keys

FORCEINLINE bool operator==(const FPointerEvent& Lhs, const FPointerEvent& Rhs)
{
	return Lhs.GetEffectingButton() == Rhs.GetEffectingButton();
}

FORCEINLINE bool operator==(const FKeyEvent& Lhs, const FKeyEvent& Rhs)
{
	return Lhs.GetKey() == Rhs.GetKey();
}

FORCEINLINE uint32 GetTypeHash(const FPointerEvent& PointerEvent)
{
	// @todo add more data from the event to the hash
	return GetTypeHash(PointerEvent.GetEffectingButton());
}

FORCEINLINE uint32 GetTypeHash(const FKeyEvent& KeyEvent)
{
	// @todo add more data from the event to the hash to support modifier keys
	return GetTypeHash(KeyEvent.GetKey());
}

struct FHeartWidgetInputTrip
{
	FKey Key;

	friend bool operator==(const FHeartWidgetInputTrip& Lhs, const FHeartWidgetInputTrip& Rhs)
	{
		return Lhs.Key == Rhs.Key;
	}

	friend bool operator!=(const FHeartWidgetInputTrip& Lhs, const FHeartWidgetInputTrip& Rhs)
	{
		return !(Lhs == Rhs);
	}
};

FORCEINLINE uint32 GetTypeHash(const FHeartWidgetInputTrip& Trip)
{
	uint32 KeyHash = 0;
	KeyHash = HashCombine(KeyHash, GetTypeHash(Trip.Key));
	//KeyHash = HashCombine(KeyHash, GetTypeHash(Trip.PointerEvent.Get(FPointerEvent())));
	//KeyHash = HashCombine(KeyHash, GetTypeHash(Trip.KeyEvent.Get(FKeyEvent())));
	return KeyHash;
}

UCLASS()
class HEARTCORE_API UHeartWidgetInputLinker : public UObject
{
	GENERATED_BODY()

public:
	struct FConditionalInputCallback
	{
		FHeartWidgetInputCondition Condition;
		FHeartWidgetLinkedInput Callback;
	};

	struct FConditionalDragDropTriggers
	{
		FHeartWidgetInputCondition Condition;
		TSubclassOf<UHeartDragDropOperation> Class;
		TSubclassOf<UUserWidget> VisualClass;
		EDragPivot Pivot;
		FVector2D Offset;
	};

	FReply HandleOnMouseWheel(UWidget* Widget, const FPointerEvent& PointerEvent);
	FReply HandleOnMouseButtonDown(UWidget* Widget, const FPointerEvent& PointerEvent);
	FReply HandleOnKeyDown(UWidget* Widget, const FKeyEvent& Key);

	UHeartDragDropOperation* HandleOnDragDetected(UWidget* Widget, const FPointerEvent& PointerEvent);
	bool HandleNativeOnDrop(UWidget* Widget, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);
	bool HandleNativeOnDragOver(UWidget* Widget, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation);

	void BindToOnKeyDown(const FHeartWidgetInputTrip& Trip, const FConditionalInputCallback& InputCallback);
	void UnbindToOnKeyDown(const FHeartWidgetInputTrip& Trip);

	void BindToOnDragDetected(const FHeartWidgetInputTrip& Trip, const FConditionalDragDropTriggers& DragDropTrigger);
	void UnbindToOnDragDetected(const FHeartWidgetInputTrip& Trip);

	TMap<FHeartWidgetInputTrip, FConditionalInputCallback> InputCallbackMappings;

	// Keys that trip a drag drop operation, paired the the class of DDO and the widget class to spawn as a visual
	TMap<FHeartWidgetInputTrip, FConditionalDragDropTriggers> DragDropTriggers;
};

USTRUCT(BlueprintType)
struct HEARTCORE_API FHeartWidgetInputTrigger
{
	GENERATED_BODY()

	virtual ~FHeartWidgetInputTrigger() {}

	virtual FHeartWidgetInputTrip CreateTrip() const { return FHeartWidgetInputTrip(); }
};

USTRUCT(BlueprintType, meta = (DisplayName = "Key"))
struct FHeartWidgetInputTrigger_Key : public FHeartWidgetInputTrigger
{
	GENERATED_BODY()

	virtual FHeartWidgetInputTrip CreateTrip() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FKey Key;
};

UCLASS(Abstract, Const, DefaultToInstanced, EditInlineNew, CollapseCategories)
class HEARTCORE_API UHeartWidgetInputCondition : public UObject
{
	GENERATED_BODY()

public:
	virtual FHeartWidgetInputCondition CreateCondition() const { return FHeartWidgetInputCondition(); }
};

UCLASS(meta = (DisplayName = "Condition - Widget Class"))
class UHeartWidgetInputCondition_WidgetClass : public UHeartWidgetInputCondition
{
	GENERATED_BODY()

public:
	virtual FHeartWidgetInputCondition CreateCondition() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UWidget> WidgetClass;
};


UCLASS(Abstract, Const, DefaultToInstanced, EditInlineNew, CollapseCategories)
class HEARTCORE_API UHeartWidgetInputEvent : public UObject
{
	GENERATED_BODY()

public:
	virtual FHeartWidgetLinkedInput CreateDelegate() const { return FHeartWidgetLinkedInput(); }
};

/**
 *
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, CollapseCategories)
class HEARTCORE_API UHeartWidgetInputBindingBase : public UObject
{
	GENERATED_BODY()

public:
	virtual bool Bind(UHeartWidgetInputLinker* Linker) { return false; }
	virtual bool Unbind(UHeartWidgetInputLinker* Linker) { return false; }

	// Optionally set condition
	UPROPERTY(EditAnywhere, Instanced, meta = (DisplayPriority = 100))
	TObjectPtr<UHeartWidgetInputCondition> Condition;
};

/**
 *
 */
UCLASS(meta = (DisplayName = "Triggered Event"))
class HEARTCORE_API UHeartWidgetInputBinding_TriggerBase : public UHeartWidgetInputBindingBase
{
	GENERATED_BODY()

public:
	virtual bool Bind(UHeartWidgetInputLinker* Linker) override;
	virtual bool Unbind(UHeartWidgetInputLinker* Linker) override;

protected:
	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<UHeartWidgetInputEvent> Event;

	UPROPERTY(EditAnywhere, meta = (BaseStruct = "HeartWidgetInputTrigger", ExcludeBaseStruct))
	TArray<FInstancedStruct> Triggers;
};

/**
 *
 */
UCLASS(meta = (DisplayName = "Drag Drop Operation"))
class HEARTCORE_API UHeartWidgetInputBinding_DragDropOperation : public UHeartWidgetInputBindingBase
{
	GENERATED_BODY()

public:
	virtual bool Bind(UHeartWidgetInputLinker* Linker) override;
	virtual bool Unbind(UHeartWidgetInputLinker* Linker) override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartDragDropOperation> OperationClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<UUserWidget> VisualClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDragPivot Pivot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2D Offset;

	UPROPERTY(EditAnywhere, meta = (BaseStruct = "HeartWidgetInputTrigger", ExcludeBaseStruct))
	TArray<FInstancedStruct> Triggers;
};

/**
 *
 */
USTRUCT()
struct HEARTCORE_API FHeartWidgetInputBindingContainer
{
	GENERATED_BODY()

	void SetLinker(UHeartWidgetInputLinker* Linker);

	UPROPERTY(EditAnywhere, Instanced)
	TArray<TObjectPtr<UHeartWidgetInputBindingBase>> Bindings;

	UPROPERTY()
	TObjectPtr<UHeartWidgetInputLinker> Linker;
};

UCLASS(BlueprintType)
class HEARTCORE_API UNamedFunctionEvent : public UHeartWidgetInputEvent
{
	GENERATED_BODY()

public:
	virtual FHeartWidgetLinkedInput CreateDelegate() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString FunctionName;
};
