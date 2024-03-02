// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Nodes/HeartGraphNodeAnnotation.h"

#include "Nodes/HeartAnnotationBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNodeAnnotation)

#define LOCTEXT_NAMESPACE "HeartGraphNodeAnnotation"

FText UHeartGraphNodeAnnotation::GetNodeTitle_Implementation(const UObject* Node) const
{
	if (auto&& TextComment = Cast<UHeartComment_Text>(Node))
	{
		return TextComment->GetCommentText();
	}
	if (auto&& ImageComment = Cast<UHeartComment_Image>(Node))
	{
		// Image annotations don't have a title.
		return FText();
	}

	return Super::GetNodeTitle_Implementation(Node);
}

FText UHeartGraphNodeAnnotation::GetPreviewNodeTitle_Implementation(const FHeartNodeSource NodeSource, const EHeartPreviewNodeNameContext Context) const
{
	auto&& Class = NodeSource.ThisClass();
	if (Class->IsChildOf<UHeartComment_Text>())
	{
		return LOCTEXT("DefaultNodeTitle_Text", "Text Comment");
	}
	if (Class->IsChildOf<UHeartComment_Image>())
	{
		return LOCTEXT("DefaultNodeTitle_Image", "Image Comment");
	}

	return Super::GetPreviewNodeTitle_Implementation(NodeSource, Context);
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