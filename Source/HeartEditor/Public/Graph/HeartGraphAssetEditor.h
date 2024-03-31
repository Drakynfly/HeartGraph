// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "WorkflowOrientedApp/WorkflowCentricApplication.h"
#include "EditorUndoClient.h"
#include "GraphEditor.h"
#include "Misc/NotifyHook.h"
#include "Toolkits/IToolkitHost.h"
#include "UObject/GCObject.h"

struct FPropertyChangedEvent;

class FWorkflowTabSpawnInfo;
class SGraphEditor;
class SHeartPalette;
class UEdGraphNode;
class UHeartGraph;
class UHeartEdGraphNode;
class FHeartGraphAssetToolbar;

namespace Heart::AssetEditor
{
	class SDetailsPanel;
	class FApplicationMode_Editor;

	class HEARTEDITOR_API FHeartGraphEditor : public FWorkflowCentricApplication, public FEditorUndoClient, public FGCObject, public FNotifyHook
	{
		friend class FApplicationMode_Editor;

	public:
		FHeartGraphEditor();
		virtual ~FHeartGraphEditor() override;

		// FGCObject
		virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
		virtual FString GetReferencerName() const override
		{
			return TEXT("FHeartGraphAssetEditor");
		}
		// --

		// FEditorUndoClient
		virtual void PostUndo(bool bSuccess) override;
		virtual void PostRedo(bool bSuccess) override;
		// --

		virtual void HandleUndoTransaction();

		// FNotifyHook
		virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;
		// --

		// IToolkit
		virtual FName GetToolkitFName() const override;
		virtual FText GetBaseToolkitName() const override;
		virtual FString GetWorldCentricTabPrefix() const override;
		virtual FLinearColor GetWorldCentricTabColorScale() const override;

		// FAssetEditorToolkit
		virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
		virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
		// --

		/** FAssetEditorToolkit interface */
		virtual void PostRegenerateMenusAndToolbars() override;

		// FWorkflowCentricApplication
		virtual void SetCurrentMode(FName NewMode) override;
		// --

	public:
		/** Edits the specified HeartGraph object */
		void InitAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UHeartGraph* InHeartGraph);

		UHeartGraph* GetHeartGraph() const { return HeartGraph; }

		TSharedPtr<FHeartGraphAssetToolbar> GetToolbar() const { return AssetToolbar; }

		auto GetGraphEditor() { return GraphEditor; }

		bool CanActivateMode(FName NewMode) const;

	protected:
		virtual void CreateToolbar();

		virtual void BindToolbarCommands();
		virtual void RefreshAsset();

		virtual FGraphAppearanceInfo GetGraphAppearanceInfo() const;
		virtual FText GetCornerText() const;

		virtual void BindGraphCommands();

		void OnDetailsPanelCreated(const TSharedRef<SDetailsPanel, ESPMode::ThreadSafe>& DetailsView);
		void OnNodePaletteCreated(const TSharedRef<SHeartPalette, ESPMode::ThreadSafe>& NodePalette);

		TSharedRef<SGraphEditor> CreateGraphWidget(const FWorkflowTabSpawnInfo& Info);

	private:
		static void UndoGraphAction();
		static void RedoGraphAction();

		static FReply OnSpawnGraphNodeByShortcut(FInputChord InChord, const FVector2D& InPosition, UEdGraph* InGraph);

	public:
		/** Gets the UI selection state of this editor */
		FName GetUISelectionState() const { return CurrentUISelection; }
		void SetUISelectionState(const FName SelectionOwner);

		virtual void ClearSelectionStateFor(const FName SelectionOwner);

	private:
		void OnCreateComment() const;
		void OnStraightenConnections() const;

	public:
		bool CanEdit() const;
		static EVisibility GetDebuggerVisibility();

		TSet<UHeartEdGraphNode*> GetSelectedHeartGraphNodes() const;
		int32 GetNumberOfSelectedNodes() const;
		bool GetBoundsForSelectedNodes(FSlateRect& Rect, float Padding) const;

	protected:
		virtual void OnSelectedNodesChanged(const TSet<UObject*>& Nodes);

	public:
		virtual void SelectSingleNode(UEdGraphNode* Node) const;

	protected:
		virtual void SelectAllNodes() const;
		virtual bool CanSelectAllNodes() const;

		virtual void DeleteNode(UEdGraphNode* Node);

		virtual void DeleteSelectedNodes();
		virtual void DeleteSelectedDuplicableNodes();
		virtual bool CanDeleteNodes() const;

		virtual void CopySelectedNodes() const;
		virtual bool CanCopyNodes() const;

		virtual void CutSelectedNodes();
		virtual bool CanCutNodes() const;

		virtual void PasteNodes();

	public:
		virtual void PasteNodesHere(const FVector2D& Location);
		virtual bool CanPasteNodes() const;

	protected:
		virtual void DuplicateNodes();
		virtual bool CanDuplicateNodes() const;

		virtual void OnNodeDoubleClicked(UEdGraphNode* Node) const;
		virtual void OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged);

	private:
		void AddInput() const;
		bool CanAddInput() const;

		void AddOutput() const;
		bool CanAddOutput() const;

		void RemovePin() const;
		bool CanRemovePin() const;

		void OnAddBreakpoint() const;
		void OnAddPinBreakpoint() const;

		bool CanAddBreakpoint() const;
		bool CanAddPinBreakpoint() const;

		void OnRemoveBreakpoint() const;
		void OnRemovePinBreakpoint() const;

		bool CanRemoveBreakpoint() const;
		bool CanRemovePinBreakpoint() const;

		void OnEnableBreakpoint() const;
		void OnEnablePinBreakpoint() const;

		bool CanEnableBreakpoint() const;
		bool CanEnablePinBreakpoint() const;

		void OnDisableBreakpoint() const;
		void OnDisablePinBreakpoint() const;

		bool CanDisableBreakpoint() const;
		bool CanDisablePinBreakpoint() const;

		void OnToggleBreakpoint() const;
		void OnTogglePinBreakpoint() const;

		bool CanToggleBreakpoint() const;
		bool CanTogglePinBreakpoint() const;

		void JumpToGraphNodeDefinition() const;
		bool CanJumpToGraphNodeDefinition() const;

		void JumpToNodeObjectDefinition() const;
		bool CanJumpToNodeObjectDefinition() const;

	protected:
		/** The HeartGraph asset being inspected */
		TObjectPtr<UHeartGraph> HeartGraph;

		TSharedPtr<FHeartGraphAssetToolbar> AssetToolbar;

		// ApplicationMode_Editor Tabs
		TSharedPtr<SGraphEditor> GraphEditor;
		TSharedPtr<SDetailsPanel> DetailsPanel;
		TSharedPtr<SHeartPalette> Palette;

	private:
		/** The current UI selection state of this editor */
		FName CurrentUISelection;
	};
}