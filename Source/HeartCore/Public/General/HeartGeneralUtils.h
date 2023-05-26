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

	UFUNCTION(BlueprintCallable, Category = "Heart|Utils|Class", meta = (DeterminesOutputType = BaseClass))
	static TArray<UClass*> GetChildClasses(const UClass* BaseClass, bool AllowAbstract);

	// Allows passing a Class into functions requiring an object.
	// Warning: This literally returns itself, just bypassing Blueprint's type safety. To get the Class Default Object,
	// use GetClassDefaultObject instead. Only use this when you specifically need a UClass stored in a UObject property.
	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Class", meta = (BlueprintAutocast, CompactNodeTitle = "->"))
	static UObject* ClassAsObject(UClass* Class);

	// Essentially the same as UKismetSystemLibrary's CastToClass, but without specifying a base class.
	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Class", meta = (BlueprintAutocast, CompactNodeTitle = "->"))
	static UClass* ObjectAsClass(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Heart|Utils|Class", meta = (DeterminesOutputType = Class))
	static const UObject* GetClassDefaultObject(UClass* Class);

	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Class")
	static bool IsClassDefaultObject(const UObject* Object);


	/*------------------------
			MATH UTILS
	------------------------*/

	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Math", meta = (DisplayName = "Clamp Vector"))
	static FVector BP_ClampVector(const FVector& Value, const FVectorBounds& Bounds);

	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Math", meta = (DisplayName = "Clamp Vector (2D)"))
	static FVector2D BP_ClampVector2D(const FVector2D& Value, const FVector2DBounds& Bounds);

	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Math")
	static FVector2D ComputeSplineTangent(const FVector2D& Start, const FVector2D& End, float Direction, float TensionMultiplier = 1.0);

	/**
	 * Return the number of edit operations we need to transform A to B.
	 */
	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Math")
	static int32 LevenshteinDistance(const FString& A, const FString& B);


	/*------------------------
			FONT UTILS
	------------------------*/

	static bool FontSupportsChar(const UFont* Font, TCHAR Char);

	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Math")
	static bool FontSupportsString(const UFont* Font, const FString& String);
};
