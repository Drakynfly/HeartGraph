// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

class UHeartGraph;

namespace Heart
{
	namespace AssetEditor
	{
		class FHeartGraphEditor;
	}

	namespace GraphUtils
	{
		static FName DefaultStyle("GraphDefault");

		bool JumpToClassDefinition(const UClass* Class);

		bool CheckForLoop(UEdGraphNode* A, UEdGraphNode* B);

		TSharedPtr<AssetEditor::FHeartGraphEditor> CreateHeartGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UHeartGraph* HeartGraph);

		TSharedPtr<AssetEditor::FHeartGraphEditor> GetHeartGraphAssetEditor(const UObject* ObjectToFocusOn);
	}
}