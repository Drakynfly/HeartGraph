// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Nodes/HeartGraphNodeAnnotation.h"

#include "Nodes/HeartAnnotationBase.h"

#define LOCTEXT_NAMESPACE "HeartGraphNodeAnnotation"

FText UHeartGraphNodeAnnotation::GetNodeTitle_Implementation(const UObject* Node, EHeartNodeNameContext Context) const
{
	switch (Context)
	{
	case EHeartNodeNameContext::NodeInstance:
		if (auto&& TextComment = Cast<UHeartComment_Text>(Node))
		{
			return TextComment->GetCommentText();
		}
		return FText();
	default:
		{
			if (auto&& TextComment = Cast<UHeartComment_Text>(Node))
			{
				return LOCTEXT("DefaultNodeTitle_Text", "Text Comment");
			}
			if (auto&& TextComment = Cast<UHeartComment_Image>(Node))
			{
				return LOCTEXT("DefaultNodeTitle_Image", "Image Comment");
			}
		}
	}

	return Super::GetNodeTitle_Implementation(Node, Context);
}

FText UHeartGraphNodeAnnotation::GetNodeCategory_Implementation(const UObject* Node) const
{
	return LOCTEXT("NodeCategory", "Annotations");
}

UHeartGraphNodeAnnotation_Image::UHeartGraphNodeAnnotation_Image()
{
#if WITH_EDITORONLY_DATA
	GetHeartGraphNodeSparseClassData()->OverrideCanCreateInEditor = true;
	GetHeartGraphNodeSparseClassData()->CanCreateInEditor = true;
#endif
}

bool UHeartGraphNodeAnnotation_Image::CanCreate_Implementation() const
{
	return false;
}

#undef LOCTEXT_NAMESPACE