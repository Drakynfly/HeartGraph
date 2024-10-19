// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartNodeLocationProxy.h"
#include "Model/HeartGraph.h"
#include "ModelView/HeartNodeLocationModifier.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartNodeLocationProxy)

UHeartGraph* UHeartNodeLocationProxy::GetHeartGraph() const
{
	return ProxiedObject->GetHeartGraph();
}

FVector2D UHeartNodeLocationProxy::GetNodeLocation(const FHeartNodeGuid& Node) const
{
	return ProxyLayer->ProxyToLocation(ProxiedObject->GetNodeLocation(Node));
}

void UHeartNodeLocationProxy::SetNodeLocation(const FHeartNodeGuid& Node, const FVector2D& Location, const bool InProgressMove)
{
	ProxiedObject->SetNodeLocation(Node, ProxyLayer->LocationToProxy(Location), InProgressMove);
}

FVector UHeartNodeLocationProxy::GetNodeLocation3D(const FHeartNodeGuid& Node) const
{
	if (auto&& Interface3D = Cast<IHeartGraphInterface3D>(ProxiedObject.GetObject()))
	{
		return ProxyLayer->ProxyToLocation3D(FVector(Interface3D->GetNodeLocation3D(Node)));
	}

	// Fallback to 2D
	return ProxyLayer->ProxyToLocation3D(FVector(ProxiedObject->GetNodeLocation(Node), 0.0f));
}

void UHeartNodeLocationProxy::SetNodeLocation3D(const FHeartNodeGuid& Node, const FVector& Location, const bool InProgressMove)
{
	if (auto&& Interface3D = Cast<IHeartGraphInterface3D>(ProxiedObject.GetObject()))
	{
		Interface3D->SetNodeLocation3D(Node, ProxyLayer->LocationToProxy3D(Location), InProgressMove);
	}
	else
	{
		// Fallback to 2D
		ProxiedObject->SetNodeLocation(Node, ProxyLayer->LocationToProxy(FVector2D(Location)), InProgressMove);
	}
}

UHeartNodeLocationProxy* UHeartNodeLocationProxy::Create(UObject* ObjectToProxy,
                                                        const TSubclassOf<UHeartNodeLocationProxy>& LocationProxyClass)
{
	if (!IsValid(ObjectToProxy) || !IsValid(LocationProxyClass))
	{
		return nullptr;
	}

	if (!ObjectToProxy->Implements<UHeartGraphInterface>())
	{
		UE_LOG(LogHeartGraph, Error, TEXT("Object passed into UHeartNodeLocationProxy::Create cannot be proxied!"))
		return nullptr;
	}

	auto&& NewLocationProxy = NewObject<UHeartNodeLocationProxy>(GetTransientPackage(), LocationProxyClass);
	if (IsValid(NewLocationProxy))
	{
		NewLocationProxy->ProxiedObject = ObjectToProxy;
	}

	return NewLocationProxy;
}

UHeartNodeLocationProxy* UHeartNodeLocationProxyLibrary::CreateNodeLocationProxy(UObject* ObjectToProxy,
                                                                                 const TSubclassOf<UHeartNodeLocationProxy> LocationProxyClass)
{
	return UHeartNodeLocationProxy::Create(ObjectToProxy, LocationProxyClass);
}