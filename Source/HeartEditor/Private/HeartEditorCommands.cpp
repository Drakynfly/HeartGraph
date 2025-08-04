// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartEditorCommands.h"

#include "HeartEditorStyle.h"
#include "Graph/HeartEdGraphSchema_Actions.h"

#include "Model/HeartGraphNode.h"

#include "Misc/ConfigCacheIni.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "HeartGraphCommands"

FHeartGraphToolbarCommands::FHeartGraphToolbarCommands()
	: TCommands<FHeartGraphToolbarCommands>("HeartGraphToolbar", LOCTEXT("HeartGraphToolbar", "Heart Graph Toolbar"), NAME_None, FHeartEditorStyle::GetStyleSetName())
{
}

void FHeartGraphToolbarCommands::RegisterCommands()
{
	UI_COMMAND(RefreshAsset, "Refresh Asset", "Refresh asset and all nodes", EUserInterfaceActionType::Button, FInputChord());
}

FHeartGraphCommands::FHeartGraphCommands()
	: TCommands<FHeartGraphCommands>("HeartGraph", LOCTEXT("HeartGraph", "Heart Graph"), NAME_None, FAppStyle::GetAppStyleSetName())
{
}

void FHeartGraphCommands::RegisterCommands()
{
	UI_COMMAND(AddInput, "Add Input", "Adds an input to the node", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddOutput, "Add Output", "Adds an output to the node", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(RemovePin, "Remove Pin", "Removes a pin from the node", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(AddPinBreakpoint, "Add Pin Breakpoint", "Adds a breakpoint to the pin", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(RemovePinBreakpoint, "Remove Pin Breakpoint", "Removes a breakpoint from the pin", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(EnablePinBreakpoint, "Enable Pin Breakpoint", "Enables a breakpoint on the pin", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DisablePinBreakpoint, "Disable Pin Breakpoint", "Disables a breakpoint on the pin", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(TogglePinBreakpoint, "Toggle Pin Breakpoint", "Toggles a breakpoint on the pin", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(JumpToGraphNodeDefinition, "Jump to Graph Node Definition", "Jumps to the graph node definition", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(JumpToNodeDefinition, "Jump to Node Object Definition", "Jumps to the node object definition", EUserInterfaceActionType::Button, FInputChord());
}

FHeartSpawnNodeCommands::FHeartSpawnNodeCommands()
	: TCommands<FHeartSpawnNodeCommands>(TEXT("HeartSpawnNodeCommands"), LOCTEXT("HeartGraph_SpawnNodes", "Heart Graph - Spawn Nodes"), NAME_None, FAppStyle::GetAppStyleSetName())
{
}

void FHeartSpawnNodeCommands::RegisterCommands()
{
	const FString ConfigSection = TEXT("HeartSpawnNodes");
	const FString SettingName = TEXT("Node");
	TArray<FString> NodeSpawns;
	GConfig->GetArray(*ConfigSection, *SettingName, NodeSpawns, GEditorPerProjectIni);

	for (int32 i = 0; i < NodeSpawns.Num(); ++i)
	{
		UClass* NodeClass = nullptr;

		FString ClassName;
		if (FParse::Value(*NodeSpawns[i], TEXT("Class="), ClassName))
		{
		    UClass* FoundClass = FindFirstObject<UClass>(*ClassName, EFindFirstObjectOptions::ExactClass, ELogVerbosity::Warning, TEXT("looking for SpawnNodes"));
			if (FoundClass && FoundClass->IsChildOf(UHeartGraphNode::StaticClass()))
			{
				NodeClass = FoundClass;
			}
		}
		else
		{
			FString CustomName;
			if (FParse::Value(*NodeSpawns[i], TEXT("Name="), CustomName))
			{
				NodeClass = UHeartGraphNode::StaticClass();
			}
		}

		if (NodeClass == nullptr)
		{
			// Matching node not found
			continue;
		}

		// Setup a UI Command for key-binding
		TSharedPtr<FUICommandInfo> CommandInfo;

		FKey Key;
		bool bShift = false;
		bool bCtrl = false;
		bool bAlt = false;

		// Parse the key-binding information
		FString KeyString;
		if (FParse::Value(*NodeSpawns[i], TEXT("Key="), KeyString))
		{
			Key = *KeyString;
		}

		if (Key.IsValid())
		{
			FParse::Bool(*NodeSpawns[i], TEXT("Shift="), bShift);
			FParse::Bool(*NodeSpawns[i], TEXT("Alt="), bAlt);
			FParse::Bool(*NodeSpawns[i], TEXT("Ctrl="), bCtrl);
		}

		FInputChord Chord(Key, EModifierKey::FromBools(bCtrl, bAlt, bShift, false));

		const FText CommandLabelText = FText::FromString(NodeClass->GetName());
		const FText Description = FText::Format(LOCTEXT("NodeSpawnDescription", "Hold down the bound keys and left click in the graph panel to spawn a {0} node."), CommandLabelText);

		FUICommandInfo::MakeCommandInfo(AsShared(), CommandInfo, FName(*NodeSpawns[i]), CommandLabelText, Description, FSlateIcon(FAppStyle::GetAppStyleSetName(), *FString::Printf(TEXT("%s.%s"), *GetContextName().ToString(), *NodeSpawns[i])), EUserInterfaceActionType::Button, Chord);

		NodeCommands.Add(NodeClass, CommandInfo);
	}
}

TSharedPtr<const FInputChord> FHeartSpawnNodeCommands::GetChordByClass(const UClass* NodeClass) const
{
	if (NodeCommands.Contains(NodeClass) && NodeCommands[NodeClass]->GetFirstValidChord()->IsValidChord())
	{
		return NodeCommands[NodeClass]->GetFirstValidChord();
	}

	return nullptr;
}

TSharedPtr<FEdGraphSchemaAction> FHeartSpawnNodeCommands::GetActionByChord(const FInputChord& InChord) const
{
	if (InChord.IsValidChord())
	{
		for (const TPair<UClass*, TSharedPtr<FUICommandInfo>>& NodeCommand : NodeCommands)
		{
			if (NodeCommand.Value.Get()->HasActiveChord(InChord))
			{
				return GetActionByClass(NodeCommand.Key);
			}
		}
	}

	return TSharedPtr<FEdGraphSchemaAction>();
}

TSharedPtr<FEdGraphSchemaAction> FHeartSpawnNodeCommands::GetActionByClass(const UClass* NodeClass) const
{
	if (NodeClass == UHeartGraphNode::StaticClass())
	{
		return MakeShared<FHeartGraphSchemaAction_NewComment>();
	}
	else
	{
		return nullptr;
		// #todo if we are going to support Chord-based node spawning we need to have a better way to register them
		//TSharedPtr<FHeartGraphSchemaAction_NewNode> NewNodeAction = MakeShared<FHeartGraphSchemaAction_NewNode>(NodeClass);
		//return NewNodeAction;
	}
}

#undef LOCTEXT_NAMESPACE