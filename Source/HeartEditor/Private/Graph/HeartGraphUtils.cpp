// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartGraphUtils.h"

#include "Graph/HeartGraphAssetEditor.h"
#include "Graph/HeartEdGraph.h"

#include "Model/HeartGraph.h"

#include "SourceCodeNavigation.h"
#include "Kismet2/KismetEditorUtilities.h"

#include "Toolkits/ToolkitManager.h"

void Heart::GraphUtils::JumpToClassDefinition(const UClass* Class)
{
	if (Class->IsNative())
	{
		if (FSourceCodeNavigation::CanNavigateToClass(Class))
		{
			const bool bSucceeded = FSourceCodeNavigation::NavigateToClass(Class);
			if (bSucceeded)
			{
				return;
			}
		}

		// Failing that, fall back to the older method which will still get the file open assuming it exists
		FString NativeParentClassHeaderPath;
		const bool bFileFound = FSourceCodeNavigation::FindClassHeaderPath(Class, NativeParentClassHeaderPath) && (IFileManager::Get().FileSize(*NativeParentClassHeaderPath) != INDEX_NONE);
		if (bFileFound)
		{
			const FString AbsNativeParentClassHeaderPath = FPaths::ConvertRelativePathToFull(NativeParentClassHeaderPath);
			FSourceCodeNavigation::OpenSourceFile(AbsNativeParentClassHeaderPath);
		}
	}
	else
	{
		FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Class);
	}
}

TSharedPtr<FHeartGraphAssetEditor> Heart::GraphUtils::GetHeartGraphAssetEditor(const UObject* ObjectToFocusOn)
{
	check(ObjectToFocusOn);

	TSharedPtr<FHeartGraphAssetEditor> HeartGraphAssetEditor;
	if (const UHeartGraph* HeartGraph = Cast<UHeartEdGraph>(ObjectToFocusOn)->GetHeartGraph())
	{
		const TSharedPtr<IToolkit> FoundAssetEditor = FToolkitManager::Get().FindEditorForAsset(HeartGraph);
		if (FoundAssetEditor.IsValid())
		{
			HeartGraphAssetEditor = StaticCastSharedPtr<FHeartGraphAssetEditor>(FoundAssetEditor);
		}
	}
	return HeartGraphAssetEditor;
}
