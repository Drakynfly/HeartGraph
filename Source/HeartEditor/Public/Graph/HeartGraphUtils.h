// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

class UHeartGraph;

namespace Heart
{
	namespace AssetEditor
	{
		class FAssetEditor;
	}

	namespace GraphUtils
	{
		void JumpToClassDefinition(const UClass* Class);

		TSharedPtr<AssetEditor::FAssetEditor> CreateHeartGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UHeartGraph* HeartGraph);

		TSharedPtr<AssetEditor::FAssetEditor> GetHeartGraphAssetEditor(const UObject* ObjectToFocusOn);
	}
}