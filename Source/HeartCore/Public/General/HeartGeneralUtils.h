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
	/************************/
	/*		CLASS UTILS		*/
	/************************/

	UFUNCTION(BlueprintCallable, Category = "Rose|Utils|Class")
	static TSubclassOf<UObject> GetParentClass(TSubclassOf<UObject> Class);

	UFUNCTION(BlueprintCallable, Category = "Rose|Utils|Class", meta = (DeterminesOutputType = Class))
	static TArray<TSubclassOf<UObject>> GetChildClasses(TSubclassOf<UObject> Class, bool AllowAbstract);

	UFUNCTION(BlueprintCallable, Category = "Rose|Utils|Class", meta = (DeterminesOutputType = Class))
	static const UObject* GetClassDefaultObject(TSubclassOf<UObject> Class);

	UFUNCTION(BlueprintPure, Category = "Rose|Utils|Class")
	static FText GetClassDisplayNameText(TSubclassOf<UObject> Class);

	UFUNCTION(BlueprintPure, Category = "Rose|Utils|Class")
	static FText GetClassTooltip(TSubclassOf<UObject> Class);


	/************************/
	/*		MATH UTILS		*/
	/************************/

	UFUNCTION(BlueprintCallable, Category = "Rose|Utils|Math", meta = (DisplayName = "Clamp Vector"))
	static FVector BP_ClampVector(const FVector& Value, const FVectorBounds& Bounds);

	UFUNCTION(BlueprintCallable, Category = "Rose|Utils|Math", meta = (DisplayName = "Clamp Vector (2D)"))
	static FVector2D BP_ClampVector2D(const FVector2D& Value, const FVector2DBounds& Bounds);
};
