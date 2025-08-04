// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Templates/SharedPointer.h"
#include "Toolkits/IToolkit.h"

class UEdGraphNode;
class UHeartGraph;

namespace Heart
{
	namespace AssetEditor
	{
		class FHeartGraphEditor;
	}

	namespace GraphUtils
	{
		static const FLazyName DefaultStyle("GraphDefault");

		bool JumpToClassDefinition(const UClass* Class);

		bool CheckForLoop(UEdGraphNode* A, UEdGraphNode* B);

		TSharedPtr<AssetEditor::FHeartGraphEditor> CreateHeartGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UHeartGraph* HeartGraph);

		TSharedPtr<AssetEditor::FHeartGraphEditor> GetHeartGraphAssetEditor(const UObject* ObjectToFocusOn);
	}
}