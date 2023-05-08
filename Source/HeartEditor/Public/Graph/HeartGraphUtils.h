// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

class UHeartGraph;
class FHeartGraphAssetEditor;

namespace Heart::GraphUtils
{
	void JumpToClassDefinition(const UClass* Class);

	TSharedRef<FHeartGraphAssetEditor> CreateHeartGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UHeartGraph* HeartGraph);

	TSharedPtr<FHeartGraphAssetEditor> GetHeartGraphAssetEditor(const UObject* ObjectToFocusOn);
}