// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

class FHeartGraphAssetEditor;

namespace Heart::GraphUtils
{
	void JumpToClassDefinition(const UClass* Class);

	TSharedPtr<FHeartGraphAssetEditor> GetHeartGraphAssetEditor(const UObject* ObjectToFocusOn);
}