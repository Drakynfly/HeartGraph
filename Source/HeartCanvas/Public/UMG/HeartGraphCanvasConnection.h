// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphWidgetBase.h"
#include "View/HeartVisualizerInterfaces.h"
#include "HeartGraphCanvasConnection.generated.h"

class UHeartGraphCanvasPin;

/**
 * A widget representing a connection between two pins
 */
UCLASS(Abstract)
class HEARTCANVAS_API UHeartGraphCanvasConnection : public UHeartGraphWidgetBase, public IGraphConnectionVisualizerInterface
{
	GENERATED_BODY()

	friend class UHeartGraphCanvasNode;

public:
	/** IHeartWidgetInputLinkerRedirector */
	virtual UHeartWidgetInputLinker* ResolveLinker_Implementation() const override final;
	/** IHeartWidgetInputLinkerRedirector */



protected:
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	TWeakObjectPtr<UHeartGraphCanvas> GraphCanvas;

	UPROPERTY(BlueprintReadOnly, Category = "Pins")
	TWeakObjectPtr<UHeartGraphCanvasPin> FromPin;

	UPROPERTY(BlueprintReadOnly, Category = "Pins")
	TWeakObjectPtr<UHeartGraphCanvasPin> ToPin;
};
