// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Model/HeartGraphNode.h"
#include "HeartGraphNodeAnnotation.generated.h"

/**
 *
 */
UCLASS()
class HEART_API UHeartGraphNodeAnnotation : public UHeartGraphNode
{
	GENERATED_BODY()

public:
	virtual FText GetPreviewNodeTitle_Implementation(FHeartNodeSource NodeSource, EHeartPreviewNodeNameContext Context) const override;
	virtual FText GetNodeTitle_Implementation(const UObject* Node) const override;
	virtual FText GetNodeCategory_Implementation(const UObject* Node) const override;
};

/**
 *
 */
UCLASS()
class HEART_API UHeartGraphNodeAnnotation_Image : public UHeartGraphNodeAnnotation
{
	GENERATED_BODY()

public:
	UHeartGraphNodeAnnotation_Image();

	virtual bool CanCreate_Implementation() const override;
};