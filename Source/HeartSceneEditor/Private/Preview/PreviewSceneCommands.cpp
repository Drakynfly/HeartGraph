// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Preview/PreviewSceneCommands.h"

#include "InputCoreTypes.h"

#define LOCTEXT_NAMESPACE "FPreviewSceneCommands"

namespace Heart::AssetEditor
{
	FPreviewSceneCommands::FPreviewSceneCommands()
	  : TCommands<FPreviewSceneCommands>(
		  "AssetEditorTemplateEditor",
		  LOCTEXT("AssetTemplateEditor",
		  "Asset Editor Template Editor"),
		  NAME_None,
		  FAppStyle::GetAppStyleSetName())
	{
	}

	void FPreviewSceneCommands::RegisterCommands()
	{
		UI_COMMAND(FocusViewport, "Focus Viewport", "Focus Viewport on Mesh", EUserInterfaceActionType::Button, FInputChord(EKeys::F));
	}
}

#undef LOCTEXT_NAMESPACE