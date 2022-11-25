// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

class FHeartGraphAssetEditor;

class HEARTEDITOR_API FHeartGraphUtils
{
public:
	FHeartGraphUtils() {}

	static TSharedPtr<FHeartGraphAssetEditor> GetHeartGraphAssetEditor(const UObject* ObjectToFocusOn);
};
