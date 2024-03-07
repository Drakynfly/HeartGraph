// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartNetUtils.h"
#include "LogHeartNet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartNetUtils)

bool UHeartNetUtils::IsObjectInActorReplicateSubObjectList(const AActor* Actor, const UObject* Object)
{
	if (IsValid(Object) && IsValid(Actor))
	{
		return Actor->IsReplicatedSubObjectRegistered(Object);
	}
	return false;
}

bool UHeartNetUtils::AddObjectToActorReplicateSubObjectList(AActor* Actor, UObject* Object)
{
	if (IsValid(Object) && IsValid(Actor))
	{
		if (Object->GetTypedOuter<AActor>() != Actor)
		{
			UE_LOG(LogHeartNet, Warning,
				TEXT("AddObjectToActorReplicateSubObjectList: Should not register Object to Actor that does not own it."
						" GivenActor: '%s', Object: '%s' DirectOuter: '%s', FirstActorOuter: '%s'"),
				*Actor->GetName(), *Object->GetName(), *Object->GetOuter()->GetName(),
				*Object->GetTypedOuter<AActor>()->GetName())
			return false;
		}

		Actor->AddReplicatedSubObject(Object);
		return Actor->IsReplicatedSubObjectRegistered(Object);
	}
	return false;
}

bool UHeartNetUtils::RemoveObjectFromActorReplicateSubObjectList(AActor* Actor, UObject* Object)
{
	if (IsValid(Object) && IsValid(Actor))
	{
		if (Actor->IsReplicatedSubObjectRegistered(Object))
		{
			Actor->RemoveReplicatedSubObject(Object);
			return true;
		}
	}
	return false;
}