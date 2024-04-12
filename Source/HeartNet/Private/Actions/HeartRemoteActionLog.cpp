// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartRemoteActionLog.h"
#include "BloodContainer.h"
#include "GraphProxy/HeartGraphNetProxy.h"
#include "ModelView/HeartActionHistory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartRemoteActionLog)

#define LOCTEXT_NAMESPACE "HeartRemoveActionLog"

const FLazyName UHeartRemoteActionLog::LogStorage("rpc_log");

FText UHeartRemoteActionLog::GetDescription(const UObject* Target) const
{
	return FText::Format(LOCTEXT("DescriptionFormat", "Remote Action on '{0}'"),
		{ FText::FromString(Target->GetName()) });
}

FHeartEvent UHeartRemoteActionLog::ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation,
	UObject* ContextObject, FBloodContainer& UndoData) const
{
	checkfSlow(Activation.IsRedoAction(), TEXT("UHeartRemoteActionLog should only be executed as a Redo!"))

	auto&& Data = UndoData.Get<FHeartRemoteActionLogUndoData>(LogStorage);
	Data.NetProxy->ExecuteRedoOnServer();

	return FHeartEvent::Handled;
}

bool UHeartRemoteActionLog::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	auto&& Data = UndoData.Get<FHeartRemoteActionLogUndoData>(LogStorage);
	Data.NetProxy->ExecuteUndoOnServer();
	return true;
}

#undef LOCTEXT_NAMESPACE