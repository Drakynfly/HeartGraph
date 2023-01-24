// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartNodeLocationAccessor.h"

#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNode3D.h"

FVector2D IHeartNodeLocationAccessor::GetNodeLocation(const FHeartNodeGuid Node) const
{
	return GetHeartGraph()->GetNode(Node)->GetLocation();
}

void IHeartNodeLocationAccessor::SetNodeLocation(const FHeartNodeGuid Node, const FVector2D& Location)
{
	GetHeartGraph()->GetNode(Node)->SetLocation(Location);
}

FVector IHeartNodeLocationAccessor::GetNodeLocation3D(const FHeartNodeGuid Node) const
{
	return GetHeartGraph()->GetNode<UHeartGraphNode3D>(Node)->GetLocation3D();
}

void IHeartNodeLocationAccessor::SetNodeLocation3D(const FHeartNodeGuid Node, const FVector& Location)
{
	GetHeartGraph()->GetNode<UHeartGraphNode3D>(Node)->SetLocation3D(Location);
}

FVector2D UHeartNodeLocationAccessorLibrary::GetNodeLocation_Pointer(
	const TScriptInterface<IHeartNodeLocationAccessor>& Accessor, UHeartGraphNode* Node)
{
	return Accessor->GetNodeLocation(Node->GetGuid());
}

void UHeartNodeLocationAccessorLibrary::SetNodeLocation_Pointer(
	const TScriptInterface<IHeartNodeLocationAccessor>& Accessor, UHeartGraphNode* Node, const FVector2D& Location)
{
	Accessor->SetNodeLocation(Node->GetGuid(), Location);
}

FVector UHeartNodeLocationAccessorLibrary::GetNodeLocation3D_Pointer(
	const TScriptInterface<IHeartNodeLocationAccessor>& Accessor, UHeartGraphNode3D* Node)
{
	return Accessor->GetNodeLocation3D(Node->GetGuid());
}

void UHeartNodeLocationAccessorLibrary::SetNodeLocation3D_Pointer(
	const TScriptInterface<IHeartNodeLocationAccessor>& Accessor, UHeartGraphNode3D* Node, const FVector& Location)
{
	Accessor->SetNodeLocation3D(Node->GetGuid(), Location);
}

const UHeartGraph* UHeartNodeLocationProxy::GetHeartGraph() const
{
	return ProxiedObject->GetHeartGraph();
}

FVector2D UHeartNodeLocationProxy::GetNodeLocation(const FHeartNodeGuid Node) const
{
	return ProxyToLocation(ProxiedObject->GetNodeLocation(Node));
}

void UHeartNodeLocationProxy::SetNodeLocation(const FHeartNodeGuid Node, const FVector2D& Location)
{
	ProxiedObject->SetNodeLocation(Node, LocationToProxy(Location));
}

FVector UHeartNodeLocationProxy::GetNodeLocation3D(const FHeartNodeGuid Node) const
{
	return ProxyToLocation3D(ProxiedObject->GetNodeLocation3D(Node));
}

void UHeartNodeLocationProxy::SetNodeLocation3D(const FHeartNodeGuid Node, const FVector& Location)
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
