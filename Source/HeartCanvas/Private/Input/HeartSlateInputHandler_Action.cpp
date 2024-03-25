// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartSlateInputHandler_Action.h"
#include "Actions/HeartGraphCanvasAction.h"
#include "Slate/SHeartGraphWidgetBase.h"
#include "UMG/HeartGraphWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSlateInputHandler_Action)

UHeartSlatePtrWrapper* UHeartSlatePtrWrapper::Wrap(const TSharedRef<SWidget>& Widget)
{
	UHeartSlatePtrWrapper* NewWrapper = nullptr;

	auto&& Metadata = Widget->GetMetaData<Heart::Canvas::FNodeAndLinkerMetadata>();
	if (!Metadata.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to find FNodeAndLinkerMetadata for slate widget!"))
		return nullptr;
	}

	switch (Metadata->WidgetType)
	{
	case Heart::Canvas::None:
		break;
	case Heart::Canvas::Pin:
		NewWrapper = NewObject<UHeartSlatePinWrapper>();
		break;
	case Heart::Canvas::Node:
		NewWrapper = NewObject<UHeartSlateNodeWrapper>();
		break;
	case Heart::Canvas::Connection:
		// @todo do we need a unique connection type?
		NewWrapper = NewObject<UHeartSlatePtrWrapper>();
		break;
	}

	if (ensure(IsValid(NewWrapper)))
	{
		NewWrapper->SlatePointer = Widget;
	}

	return NewWrapper;
}

UHeartGraphNode* UHeartSlateNodeWrapper::GetHeartGraphNode() const
{
	auto&& Metadata = SlatePointer->GetMetaData<Heart::Canvas::FNodeAndLinkerMetadata>();
	if (!Metadata.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to find FNodeAndLinkerMetadata for slate widget!"))
		return nullptr;
	}

	return Metadata->Node.Get();
}

UHeartGraphNode* UHeartSlatePinWrapper::GetHeartGraphNode() const
{
	auto&& Metadata = SlatePointer->GetMetaData<Heart::Canvas::FNodeAndLinkerMetadata>();
	if (!Metadata.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to find FNodeAndLinkerMetadata for slate widget!"))
		return nullptr;
	}

	return Metadata->Node.Get();
}

FText UHeartSlateInputHandler_Action::GetDescription(const TSharedRef<SWidget>& TestWidget) const
{
	if (IsValid(ActionClass))
	{
		auto&& Wrapper = UHeartSlatePtrWrapper::Wrap(TestWidget);
		return ActionClass.GetDefaultObject()->GetDescription(Wrapper);
	}

	return Super::GetDescription(TestWidget);
}

bool UHeartSlateInputHandler_Action::PassCondition(const TSharedRef<SWidget>& TestWidget) const
{
	bool Failed = !Super::PassCondition(TestWidget);

	if (IsValid(ActionClass))
	{
		auto&& Wrapper = UHeartSlatePtrWrapper::Wrap(TestWidget);
		Failed |= !ActionClass.GetDefaultObject()->CanExecute(Wrapper);
	}

	return !Failed;
}

FReply UHeartSlateInputHandler_Action::OnTriggered(TSharedRef<SWidget>& Widget, const FHeartInputActivation& Activation) const
{
	auto&& Action = UHeartGraphActionBase::CreateGraphAction(ActionClass);

	Heart::Action::FArguments Args;
	Args.Target = UHeartSlatePtrWrapper::Wrap(Widget);
	Args.Activation = Activation;

	if (Action->Execute(Args))
	{
		return FReply::Handled();
	}

	return FReply::Unhandled();
}