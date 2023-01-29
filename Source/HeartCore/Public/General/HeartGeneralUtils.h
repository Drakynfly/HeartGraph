// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "VectorBounds.h"
#include "Vector2DBounds.h"
#include "HeartGeneralUtils.generated.h"

/**
 *
 */
UCLASS()
class HEARTCORE_API UHeartGeneralUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/*------------------------
			OBJECT UTILS
	------------------------*/

	UFUNCTION(BlueprintCallable, Category = "Heart|Utils|Class", DisplayName = "Duplicate Object", meta = (DeterminesOutputType = Source))
	static UObject* K2_DuplicateObject(UObject* Outer, UObject* Source);


	/*------------------------
			CLASS UTILS
	------------------------*/

	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Class")
	static UClass* GetParentClass(const UClass* Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|Utils|Class", meta = (DeterminesOutputType = Class))
	static TArray<UClass*> GetChildClasses(const UClass* Class, bool AllowAbstract);

	// Allows passing a Class into functions requiring an object.
	// Warning: This literally returns itself, just bypassing Blueprint's type safety. To get the Class Default Object,
	// use GetClassDefaultObject instead. Only use this when you specifically need a UClass stored in a UObject property.
	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Class", meta = (BlueprintAutocast, CompactNodeTitle = "->"))
	static UObject* ClassAsObject(UClass* Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|Utils|Class", meta = (DeterminesOutputType = Class))
	static const UObject* GetClassDefaultObject(UClass* Class);

	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Class")
	static FText GetClassDisplayNameText(const UClass* Class);

	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Class")
	static FText GetClassTooltip(const UClass* Class);


	/*------------------------
			MATH UTILS
	------------------------*/

	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Math", meta = (DisplayName = "Clamp Vector"))
	static FVector BP_ClampVector(const FVector& Value, const FVectorBounds& Bounds);

	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Math", meta = (DisplayName = "Clamp Vector (2D)"))
	static FVector2D BP_ClampVector2D(const FVector2D& Value, const FVector2DBounds& Bounds);

	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Math")
	static FVector2D ComputeSplineTangent(const FVector2D& Start, const FVector2D& End, float Direction, float TensionMultiplier = 1.0);
};
