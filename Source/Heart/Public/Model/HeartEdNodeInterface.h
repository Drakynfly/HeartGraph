// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#if WITH_EDITOR
namespace Heart
{
	/**
	 * Editor-only interface to call certain functions on HeartEdGraphNodes from runtime modules
	 */
	class HEART_API IEdNodeInterface
	{
	public:
		virtual ~IEdNodeInterface() = default;
		virtual void OnPropertyChanged() = 0;
	};
}
#endif