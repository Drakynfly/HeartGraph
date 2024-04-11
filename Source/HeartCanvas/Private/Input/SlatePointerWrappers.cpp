// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/SlatePointerWrappers.h"
#include "HeartCanvasLog.h"
#include "Slate/SHeartGraphWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SlatePointerWrappers)

UHeartSlatePtr* UHeartSlatePtr::Wrap(const TSharedRef<SWidget>& Widget)
{
	UHeartSlatePtr* NewWrapper = nullptr;

	auto&& Metadata = Widget->GetMetaData<Heart::Canvas::FLinkerMetadata>();
	if (!Metadata.IsValid())
	{
		UE_LOG(LogHeartCanvas, Warning, TEXT("Unable to find FLinkerMetadata for slate widget!"))
		return nullptr;
	}

	switch (Metadata->WidgetType)
	{
	case Heart::Canvas::Graph:
		NewWrapper = NewObject<UHeartSlateGraph>();
		break;
	case Heart::Canvas::Node:
		NewWrapper = NewObject<UHeartSlateNode>();
		break;
	case Heart::Canvas::Pin:
		NewWrapper = NewObject<UHeartSlatePin>();
		break;
	case Heart::Canvas::None:
	case Heart::Canvas::Connection:
		// @todo do we need a unique type for connections ?
		NewWrapper = NewObject<UHeartSlatePtr>();
		break;
	}

	if (ensure(IsValid(NewWrapper)))
	{
		NewWrapper->SlatePointer = Widget;
	}

	return NewWrapper;
}

UHeartGraph* UHeartSlateGraph::GetHeartGraph() const
{
	auto&& Metadata = SlatePointer->GetMetaData<Heart::Canvas::FGraphAndLinkerMetadata>();
	if (!Metadata.IsValid())
	{
		UE_LOG(LogHeartCanvas, Warning, TEXT("Unable to find FGraphAndLinkerMetadata for slate widget!"))
		return nullptr;
	}

	return Metadata->Graph.Get();
}

UHeartGraphNode* UHeartSlateNode::GetHeartGraphNode() const
{
	auto&& Metadata = SlatePointer->GetMetaData<Heart::Canvas::FNodeAndLinkerMetadata>();
	if (!Metadata.IsValid())
	{
		UE_LOG(LogHeartCanvas, Warning, TEXT("Unable to find FNodeAndLinkerMetadata for slate widget!"))
		return nullptr;
	}

	return Metadata->Node.Get();
}

UHeartGraphNode* UHeartSlatePin::GetHeartGraphNode() const
{
	auto&& Metadata = SlatePointer->GetMetaData<Heart::Canvas::FPinAndLinkerMetadata>();
	if (!Metadata.IsValid())
	{
		UE_LOG(LogHeartCanvas, Warning, TEXT("Unable to find FPinAndLinkerMetadata for slate widget!"))
		return nullptr;
	}

	return Metadata->Node.Get();
}

FHeartPinGuid UHeartSlatePin::GetPinGuid() const
{
	auto&& Metadata = SlatePointer->GetMetaData<Heart::Canvas::FPinAndLinkerMetadata>();
	if (!Metadata.IsValid())
	{
		UE_LOG(LogHeartCanvas, Warning, TEXT("Unable to find FPinAndLinkerMetadata for slate widget!"))
		return FHeartPinGuid();
	}

	return Metadata->Pin;
}