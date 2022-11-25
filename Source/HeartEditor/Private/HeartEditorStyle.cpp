// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartEditorStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

TSharedPtr<FSlateStyleSet> FHeartEditorStyle::StyleSet = nullptr;

FName FHeartEditorStyle::GetStyleSetName()
{
	static FName HeartEditorStyleName(TEXT("HeartEditorStyle"));
	return HeartEditorStyleName;
}

void FHeartEditorStyle::Initialize()
{
	StyleSet = MakeShareable(new FSlateStyleSet(TEXT("HeartEditorStyle")));

	const FVector2D Icon16(16.0f, 16.0f);
	const FVector2D Icon30(30.0f, 30.0f);
	const FVector2D Icon40(40.0f, 40.0f);
	const FVector2D Icon64(64.0f, 64.0f);

	// engine assets
	StyleSet->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate/"));

	//StyleSet->Set("HeartToolbar.GoToParentInstance", new IMAGE_BRUSH("Icons/icon_DebugStepOut_40x", Icon40));

	StyleSet->Set("HeartGraph.BreakpointEnabled", new IMAGE_BRUSH("Old/Kismet2/Breakpoint_Valid", FVector2D(24.0f, 24.0f)));
	StyleSet->Set("HeartGraph.BreakpointDisabled", new IMAGE_BRUSH("Old/Kismet2/Breakpoint_Disabled", FVector2D(24.0f, 24.0f)));
	StyleSet->Set("HeartGraph.BreakpointHit", new IMAGE_BRUSH("Old/Kismet2/IP_Breakpoint", Icon40));
	StyleSet->Set("HeartGraph.PinBreakpointHit", new IMAGE_BRUSH("Old/Kismet2/IP_Breakpoint", Icon30));

	StyleSet->Set("GraphEditor.Sequence_16x", new IMAGE_BRUSH("Icons/icon_Blueprint_Sequence_16x", Icon16));

	// Heart assets
	StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("Heart"))->GetBaseDir() / TEXT("Resources"));

	StyleSet->Set("ClassIcon.HeartAsset", new IMAGE_BRUSH(TEXT("Icons/HeartAsset_16x"), Icon16));
	StyleSet->Set("ClassThumbnail.HeartAsset", new IMAGE_BRUSH(TEXT("Icons/HeartAsset_64x"), Icon64));

	StyleSet->Set("HeartToolbar.RefreshAsset", new IMAGE_BRUSH("Icons/Refresh", Icon40));

	StyleSet->Set("Heart.Node.Title", new BOX_BRUSH("Icons/HeartNode_Title", FMargin(8.0f/64.0f, 0, 0, 0)));
	StyleSet->Set("Heart.Node.Body", new BOX_BRUSH("Icons/HeartNode_Body", FMargin(16.f/64.f)));
	StyleSet->Set("Heart.Node.ActiveShadow", new BOX_BRUSH("Icons/HeartNode_Shadow_Active", FMargin(18.0f/64.0f)));
	StyleSet->Set("Heart.Node.WasActiveShadow", new BOX_BRUSH("Icons/HeartNode_Shadow_WasActive", FMargin(18.0f/64.0f)));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
};

void FHeartEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
	ensure(StyleSet.IsUnique());
	StyleSet.Reset();
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
