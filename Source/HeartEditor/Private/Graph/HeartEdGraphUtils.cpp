// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartEdGraphUtils.h"

#include "HeartEditorModule.h"
#include "Graph/HeartGraphAssetEditor.h"
#include "Graph/HeartEdGraph.h"

#include "Model/HeartGraph.h"

#include "SourceCodeNavigation.h"
#include "Kismet2/KismetEditorUtilities.h"

#include "Toolkits/ToolkitManager.h"

namespace Heart::GraphUtils
{
	bool JumpToClassDefinition(const UClass* Class)
	{
		if (Class->IsNative())
		{
			if (FSourceCodeNavigation::CanNavigateToClass(Class))
			{
				const bool bSucceeded = FSourceCodeNavigation::NavigateToClass(Class);
				if (bSucceeded)
				{
					return true;
				}
			}

			// Failing that, fall back to the older method which will still get the file open assuming it exists
			FString NativeParentClassHeaderPath;
			const bool bFileFound = FSourceCodeNavigation::FindClassHeaderPath(Class, NativeParentClassHeaderPath) &&
				(IFileManager::Get().FileSize(*NativeParentClassHeaderPath) != INDEX_NONE);
			if (bFileFound)
			{
				const FString AbsNativeParentClassHeaderPath = FPaths::ConvertRelativePathToFull(NativeParentClassHeaderPath);
				return FSourceCodeNavigation::OpenSourceFile(AbsNativeParentClassHeaderPath);
			}
		}
		else
		{
			// @todo unknown if this succeeds, it doesnt return a success value
			FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Class);
			return true;
		}

		return false;
	}

	bool CheckForLoop(UEdGraphNode* A, UEdGraphNode* B)
	{
		// This is copied from a private engine class found in both EdGraphSchema_BehaviorTree.cpp and ConversationGraphSchema.cpp

		class FNodeVisitorCycleChecker
		{
		public:
			/** Check whether a loop in the graph would be caused by linking the passed-in nodes */
			bool CheckForLoop(UEdGraphNode* StartNode, UEdGraphNode* EndNode)
			{
				VisitedNodes.Add(EndNode);
				return TraverseInputNodesToRoot(StartNode);
			}

		private:
			/**
			 * Helper function for CheckForLoop()
			 * @param	Node	The node to start traversal at
			 * @return true if we reached a root node (i.e. a node with no input pins), false if we encounter a node we have already seen
			 */
			bool TraverseInputNodesToRoot(UEdGraphNode* Node)
			{
				VisitedNodes.Add(Node);

				// Follow every input pin until we cant any more ('root') or we reach a node we have seen (cycle)
				for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
				{
					UEdGraphPin* MyPin = Node->Pins[PinIndex];

					if (MyPin->Direction == EGPD_Input)
					{
						for (int32 LinkedPinIndex = 0; LinkedPinIndex < MyPin->LinkedTo.Num(); ++LinkedPinIndex)
						{
							UEdGraphPin* OtherPin = MyPin->LinkedTo[LinkedPinIndex];
							if (OtherPin)
							{
								UEdGraphNode* OtherNode = OtherPin->GetOwningNode();
								if (VisitedNodes.Contains(OtherNode))
								{
									return false;
								}
								else
								{
									return TraverseInputNodesToRoot(OtherNode);
								}
							}
						}
					}
				}

				return true;
			}

			TSet<UEdGraphNode*> VisitedNodes;
		};

		return FNodeVisitorCycleChecker().CheckForLoop(A, B);
	}

	TSharedPtr<AssetEditor::FHeartGraphEditor> CreateHeartGraphAssetEditor(const EToolkitMode::Type Mode,
		const TSharedPtr<IToolkitHost>& InitToolkitHost, UHeartGraph* HeartGraph)
	{
		if (!HeartGraph->GetEdGraph())
		{
			UE_LOG(LogHeartEditor, Error, TEXT("HeartEdGraph is invalid for HeartGraph '%s'!"), *HeartGraph->GetName())
			//return nullptr;
		}

		TSharedRef<AssetEditor::FHeartGraphEditor> NewHeartGraphAssetEditor(new AssetEditor::FHeartGraphEditor());
		NewHeartGraphAssetEditor->InitAssetEditor(Mode, InitToolkitHost, HeartGraph);
		return NewHeartGraphAssetEditor;
	}

	TSharedPtr<AssetEditor::FHeartGraphEditor> GetHeartGraphAssetEditor(const UObject* ObjectToFocusOn)
	{
		check(ObjectToFocusOn);

		TSharedPtr<AssetEditor::FHeartGraphEditor> HeartGraphAssetEditor;
		if (const UHeartGraph* HeartGraph = Cast<UHeartEdGraph>(ObjectToFocusOn)->GetHeartGraph())
		{
			const TSharedPtr<IToolkit> FoundAssetEditor = FToolkitManager::Get().FindEditorForAsset(HeartGraph);
			if (FoundAssetEditor.IsValid())
			{
				HeartGraphAssetEditor = StaticCastSharedPtr<AssetEditor::FHeartGraphEditor>(FoundAssetEditor);
			}
		}
		return HeartGraphAssetEditor;
	}
}