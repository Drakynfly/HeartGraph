// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartNodeLocationAccessor.h"

#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNode3D.h"

FVector2D IHeartNodeLocationAccessor::GetNodeLocation(UHeartGraphNode* Node) const
{
	return Node->GetLocation();
}

void IHeartNodeLocationAccessor::SetNodeLocation(UHeartGraphNode* Node, const FVector2D& Location)
{
	Node->SetLocation(Location);
}

FVector IHeartNodeLocationAccessor::GetNodeLocation3D(UHeartGraphNode3D* Node) const
{
	return Node->GetLocation3D();
}

void IHeartNodeLocationAccessor::SetNodeLocation3D(UHeartGraphNode3D* Node, const FVector& Location)
{
	Node->SetLocation3D(Location);
}

const UHeartGraph* UHeartNodeLocationProxy::GetHeartGraph() const
{
	return ProxiedObject->GetHeartGraph();
}

FVector2D UHeartNodeLocationProxy::GetNodeLocation(UHeartGraphNode* Node) const
{
	return ProxyToLocation(ProxiedObject->GetNodeLocation(Node));
}

void UHeartNodeLocationProxy::SetNodeLocation(UHeartGraphNode* Node, const FVector2D& Location)
{
	ProxiedObject->SetNodeLocation(Node, LocationToProxy(Location));
}

FVector UHeartNodeLocationProxy::GetNodeLocation3D(UHeartGraphNode3D* Node) const
{
	return ProxyToLocation3D(ProxiedObject->GetNodeLocation3D(Node));
}

void UHeartNodeLocationProxy::SetNodeLocation3D(UHeartGraphNode3D* Node, const FVector& Location)
{
	ProxiedObject->SetNodeLocation3D(Node, LocationToProxy3D(Location));
}

FVector2D UHeartNodeLocationProxy::LocationToProxy(const FVector2D& Location) const
{
	return Location;
}

FVector2D UHeartNodeLocationProxy::ProxyToLocation(const FVector2D& Proxy) const
{
	return Proxy;
}

FVector UHeartNodeLocationProxy::LocationToProxy3D(const FVector& Location) const
{
	return Location;
}

FVector UHeartNodeLocationProxy::ProxyToLocation3D(const FVector& Proxy) const
{
	return Proxy;
}

UHeartNodeLocationProxy* UHeartNodeLocationProxy::Create(UObject* ObjectToProxy,
                                                        const TSubclassOf<UHeartNodeLocationProxy> LocationProxyClass)
{
	if (!IsValid(ObjectToProxy) || !IsValid(LocationProxyClass))
	{
		return nullptr;
	}

	if (!ObjectToProxy->Implements<UHeartNodeLocationAccessor>())
	{
		UE_LOG(LogTemp, Error, TEXT("Object passed into UHeartNodeLocationProxy::Create cannot be proxied!"))
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
