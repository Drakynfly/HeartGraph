// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.


#include "Nodes/HeartAnnotationBase.h"

FText UHeartComment_Text::GetCommentText() const
{
	return CommentText;
}

void UHeartComment_Text::SetCommentText(const FText& Text)
{
	CommentText = Text;
}

TSoftObjectPtr<UTexture2D> UHeartComment_Image::GetCommentImage() const
{
	return CommentImage;
}

void UHeartComment_Image::SetCommentImage(const TSoftObjectPtr<UTexture2D>& Image)
{
	CommentImage = Image;
}