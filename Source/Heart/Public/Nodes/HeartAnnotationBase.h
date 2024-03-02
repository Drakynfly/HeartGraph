// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartAnnotationBase.generated.h"

/**
 * Base class for annotation objects
 */
UCLASS(Abstract, BlueprintType)
class HEART_API UHeartAnnotationBase : public UObject
{
	GENERATED_BODY()
};

/**
 * Default implementation for a Text Comment. Can be added to any graph type
 */
UCLASS()
class HEART_API UHeartComment_Text : public UHeartAnnotationBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|TextComment")
	FText GetCommentText() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|TextComment")
	void SetCommentText(const FText& Text);

protected:
	UPROPERTY(EditInstanceOnly, Category = "Heart|TextComment")
	FText CommentText;
};


/**
 * Default implementation for a comment that displays a textures. Can be added to any graph type
 */
UCLASS()
class HEART_API UHeartComment_Image : public UHeartAnnotationBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|ImageComment")
	TSoftObjectPtr<UTexture2D> GetCommentImage() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|ImageComment")
	void SetCommentImage(const TSoftObjectPtr<UTexture2D>& Image);

protected:
	UPROPERTY(EditInstanceOnly, Category = "Heart|ImageComment")
	TSoftObjectPtr<UTexture2D> CommentImage;
};