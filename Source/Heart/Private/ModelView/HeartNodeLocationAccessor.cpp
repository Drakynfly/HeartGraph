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

FVector2D UHeartNodeLocationModifierStack::LocationToProxy(const FVector2D& Location) const
{
	FVector2D Proxy = Location;

	for (int32 i = 0; i < Modifiers.Num(); ++i)
	{
		Proxy = Modifiers[i]->LocationToProxy(Proxy);
	}

	return Proxy;
}

FVector2D UHeartNodeLocationModifierStack::ProxyToLocation(const FVector2D& Proxy) const
{
	FVector2D Location = Proxy;

	for (int32 i = Modifiers.Num() - 1; i >= 0; --i)
	{
		Location = Modifiers[i]->ProxyToLocation(Location);
	}

	return Location;
}

FVector UHeartNodeLocationModifierStack::LocationToProxy3D(const FVector& Location) const
{
	FVector Proxy = Location;

	for (int32 i = 0; i < Modifiers.Num(); ++i)
	{
		Proxy = Modifiers[i]->LocationToProxy3D(Proxy);
	}

	return Proxy;
}

FVector UHeartNodeLocationModifierStack::ProxyToLocation3D(const FVector& Proxy) const
{
	FVector Location = Proxy;

	for (int32 i = Modifiers.Num() - 1; i >= 0; --i)
	{
		Location = Modifiers[i]->ProxyToLocation3D(Location);
	}

	return Location;
}

UHeartGraph* UHeartNodeLocationProxy::GetHeartGraph() const
{
	return ProxiedObject->GetHeartGraph();
}

FVector2D UHeartNodeLocationProxy::GetNodeLocation(const FHeartNodeGuid Node) const
{
	return ProxyLayer->ProxyToLocation(ProxiedObject->GetNodeLocation(Node));
}

void UHeartNodeLocationProxy::SetNodeLocation(const FHeartNodeGuid Node, const FVector2D& Location)
{
	ProxiedObject->SetNodeLocation(Node, ProxyLayer->LocationToProxy(Location));
}

FVector UHeartNodeLocationProxy::GetNodeLocation3D(const FHeartNodeGuid Node) const
{
	return ProxyLayer->ProxyToLocation3D(ProxiedObject->GetNodeLocation3D(Node));
}

void UHeartNodeLocationProxy::SetNodeLocation3D(const FHeartNodeGuid Node, const FVector& Location)
{
	ProxiedObject->SetNodeLocation3D(Node, ProxyLayer->LocationToProxy3D(Location));
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
