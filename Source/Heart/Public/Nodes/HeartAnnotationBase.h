// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartAnnotationBase.generated.h"

/**
 *
 */
UCLASS(Abstract, BlueprintType)
class HEART_API UHeartAnnotationBase : public UObject
{
	GENERATED_BODY()
};

UCLASS()
class HEART_API UHeartComment_Text : public UHeartAnnotationBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|CommentText")
	FText GetCommentText() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|CommentText")
	void SetCommentText(const FText& Text);

protected:
	UPROPERTY(EditInstanceOnly)
	FText CommentText;
};

UCLASS()
class HEART_API UHeartComment_Image : public UHeartAnnotationBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|CommentImage")
	TSoftObjectPtr<UTexture2D> GetCommentImage() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|CommentImage")
	void SetCommentImage(const TSoftObjectPtr<UTexture2D>& Image);

protected:
	UPROPERTY(EditInstanceOnly)
	TSoftObjectPtr<UTexture2D> CommentImage;
};
