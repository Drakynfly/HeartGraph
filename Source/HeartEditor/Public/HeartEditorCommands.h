// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandInfo.h"
#include "Templates/SharedPointer.h"

class HEARTEDITOR_API FHeartGraphToolbarCommands final : public TCommands<FHeartGraphToolbarCommands>
{
public:
	FHeartGraphToolbarCommands();

	TSharedPtr<FUICommandInfo> RefreshAsset;

	virtual void RegisterCommands() override;
};

/** Generic graph commands for the heart graph */
class FHeartGraphCommands final : public TCommands<FHeartGraphCommands>
{
public:
	FHeartGraphCommands();

	/** Context Pins */
	TSharedPtr<FUICommandInfo> RefreshContextPins;

	/** Pins */
	TSharedPtr<FUICommandInfo> AddInput;
	TSharedPtr<FUICommandInfo> AddOutput;
	TSharedPtr<FUICommandInfo> RemovePin;

	/** Breakpoints */
	TSharedPtr<FUICommandInfo> AddPinBreakpoint;
	TSharedPtr<FUICommandInfo> RemovePinBreakpoint;
	TSharedPtr<FUICommandInfo> EnablePinBreakpoint;
	TSharedPtr<FUICommandInfo> DisablePinBreakpoint;
	TSharedPtr<FUICommandInfo> TogglePinBreakpoint;

	/** Jumps */
	TSharedPtr<FUICommandInfo> JumpToNodeDefinition;

	virtual void RegisterCommands() override;
};

/** Handles spawning nodes by keyboard shortcut */
class FHeartSpawnNodeCommands : public TCommands<FHeartSpawnNodeCommands>
{
public:
	FHeartSpawnNodeCommands();

	virtual void RegisterCommands() override;

	TSharedPtr<const FInputChord> GetChordByClass(const UClass* NodeClass) const;
	TSharedPtr<FEdGraphSchemaAction> GetActionByChord(const FInputChord& InChord) const;

private:
	TSharedPtr<FEdGraphSchemaAction> GetActionByClass(UClass* NodeClass) const;

	TMap<UClass*, TSharedPtr<FUICommandInfo>> NodeCommands;
};
