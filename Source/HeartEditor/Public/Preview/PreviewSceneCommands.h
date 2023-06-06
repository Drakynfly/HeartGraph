// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Styling/AppStyle.h"
#include "Framework/Commands/Commands.h"
#include "Templates/SharedPointer.h"

class FUICommandInfo;

namespace Heart::AssetEditor
{
	class FPreviewSceneCommands : public TCommands<FPreviewSceneCommands>
	{
	public:
		FPreviewSceneCommands();

		/** Focuses Viewport on Mesh */
		TSharedPtr<FUICommandInfo> FocusViewport;

		/** Initialize commands */
		virtual void RegisterCommands() override;
	};
}


