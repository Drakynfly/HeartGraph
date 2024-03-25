// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartSlateInputBinding.h"
#include "View/HeartVisualizerInterfaces.h"
#include "HeartSlateInputHandler_Action.generated.h"

class UHeartGraphActionBase;

/**
 * This object is a wrapper around a SWidget shared pointer, to allow it to be passed to graph actions.
 */
UCLASS()
class UHeartSlatePtrWrapper : public UObject
{
	GENERATED_BODY()

public:
	static UHeartSlatePtrWrapper* Wrap(const TSharedRef<SWidget>& Widget);

protected:
	TSharedPtr<SWidget> SlatePointer;
};

/**
 * Child of PtrWrapper, whose sole purpose is to pass checks for implementing IGraphNodeVisualizerInterface
 */
UCLASS()
class UHeartSlateNodeWrapper : public UHeartSlatePtrWrapper, public IGraphNodeVisualizerInterface
{
	GENERATED_BODY()

public:
	virtual UHeartGraphNode* GetHeartGraphNode() const override;
};

/**
 * Child of PtrWrapper, whose sole purpose is to pass checks for implementing IGraphPinVisualizerInterface
 */
UCLASS()
class UHeartSlatePinWrapper : public UHeartSlatePtrWrapper, public IGraphPinVisualizerInterface
{
	GENERATED_BODY()

public:
	virtual UHeartGraphNode* GetHeartGraphNode() const override;
};

UCLASS(meta = (DisplayName = "Graph Action (slate)"))
class HEARTCANVAS_API UHeartSlateInputHandler_Action : public UHeartSlateInputHandlerBase
{
	GENERATED_BODY()

public:
	virtual FText GetDescription(const TSharedRef<SWidget>& TestWidget) const override;
	virtual bool PassCondition(const TSharedRef<SWidget>& TestWidget) const override;
	virtual FReply OnTriggered(TSharedRef<SWidget>& Widget, const FHeartInputActivation& Activation) const override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartGraphActionBase> ActionClass;
};