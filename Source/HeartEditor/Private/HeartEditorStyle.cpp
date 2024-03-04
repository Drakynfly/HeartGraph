// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartEditorStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define IMAGE_BRUSH_SVG( RelativePath, ... ) FSlateVectorImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".svg")), __VA_ARGS__)

TSharedPtr<FSlateStyleSet> FHeartEditorStyle::StyleSet = nullptr;

FName FHeartEditorStyle::GetStyleSetName()
{
	static FName HeartEditorStyleName(TEXT("HeartEditorStyle"));
	return HeartEditorStyleName;
}

void FHeartEditorStyle::Initialize()
{
	StyleSet = MakeShared<FSlateStyleSet>(TEXT("HeartEditorStyle"));

	const FVector2D Icon16(16.0f, 16.0f);
	const FVector2D Icon20(20.0f, 20.0f);
	const FVector2D Icon24(24.0f, 24.0f);
	const FVector2D Icon30(30.0f, 30.0f);
	const FVector2D Icon40(40.0f, 40.0f);
	const FVector2D Icon64(64.0f, 64.0f);

	// engine assets
	StyleSet->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate/"));

	StyleSet->Set("HeartGraphToolbar.RefreshAsset", new IMAGE_BRUSH_SVG("Starship/Common/Apply", Icon20));
	StyleSet->Set("HeartGraphToolbar.ValidateAsset", new IMAGE_BRUSH_SVG("Starship/Common/Debug", Icon20));

	StyleSet->Set("HeartToolbar.SearchInAsset", new IMAGE_BRUSH_SVG("Starship/Common/Search", Icon20));
	StyleSet->Set("HeartToolbar.EditAssetDefaults", new IMAGE_BRUSH_SVG("Starship/Common/Details", Icon20));

	StyleSet->Set("HeartGraph.BreakpointEnabled", new IMAGE_BRUSH("Old/Kismet2/Breakpoint_Valid", Icon24));
	StyleSet->Set("HeartGraph.BreakpointDisabled", new IMAGE_BRUSH("Old/Kismet2/Breakpoint_Disabled", Icon24));
	StyleSet->Set("HeartGraph.BreakpointHit", new IMAGE_BRUSH("Old/Kismet2/IP_Breakpoint", Icon40));
	StyleSet->Set("HeartGraph.PinBreakpointHit", new IMAGE_BRUSH("Old/Kismet2/IP_Breakpoint", Icon30));

	// Heart assets
	StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("Heart"))->GetBaseDir() / TEXT("Resources"));

	StyleSet->Set("ClassIcon.HeartAsset", new IMAGE_BRUSH(TEXT("Icons/HeartAsset_16x"), Icon16));
	StyleSet->Set("ClassThumbnail.HeartAsset", new IMAGE_BRUSH(TEXT("Icons/HeartAsset_64x"), Icon64));

	StyleSet->Set("Heart.Node.Title", new BOX_BRUSH("Icons/HeartNode_Title", FMargin(8.0f/64.0f, 0, 0, 0)));
	StyleSet->Set("Heart.Node.Body", new BOX_BRUSH("Icons/HeartNode_Body", FMargin(16.f/64.f)));
	StyleSet->Set("Heart.Node.ActiveShadow", new BOX_BRUSH("Icons/HeartNode_Shadow_Active", FMargin(18.0f/64.0f)));
	StyleSet->Set("Heart.Node.WasActiveShadow", new BOX_BRUSH("Icons/HeartNode_Shadow_WasActive", FMargin(18.0f/64.0f)));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

void FHeartEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
	ensure(StyleSet.IsUnique());
	StyleSet.Reset();
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef IMAGE_BRUSH_SVG