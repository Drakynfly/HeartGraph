// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartWidgetInputLinkerRedirector.generated.h"

// This class does not need to be modified.
UINTERFACE()
class HEARTCORE_API UHeartWidgetInputLinkerRedirector : public UInterface
{
	GENERATED_BODY()
};

/**
 * Add this to widgets that don't have their own UHeartWidgetInputLinker, but know how to get one, and want to route
 * their input through it.
 */
class HEARTCORE_API IHeartWidgetInputLinkerRedirector
{
	GENERATED_BODY()

public:
	/**
	 * Get the Input Linker for this widget, or null if it doesn't have one.
	 * It is supported, and in fact, encouraged, to implement this by deferring to another widget's implementation:
	 *
	 * {
	 *     return Execute_ResolveLinker(SomeWidget);
     * }
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|WidgetInputLinkerRedirector")
	class UHeartWidgetInputLinker* ResolveLinker() const;
};
