// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Assets/HeartInputHandlerAssetFactory.h"
#include "Assets/HeartDefaultClassFilter.h"

#include "Input/HeartInputHandlerAssetBase.h"

#include "ClassViewerModule.h"
#include "Editor.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "SlateOptMacros.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SWindow.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartInputHandlerAssetFactory)

#define LOCTEXT_NAMESPACE "HeartInputHandlerAssetFactory"

// ------------------------------------------------------------------------------
// Dialog to configure creation properties
// ------------------------------------------------------------------------------
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

class SHeartInputHandlerAssetCreateDialog final : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHeartInputHandlerAssetCreateDialog) {}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs)
	{
		bOkClicked = false;
		SelectedClass = UHeartInputHandlerAssetBase::StaticClass();

		ChildSlot
		[
			SNew(SBorder)
				.Visibility(EVisibility::Visible)
				.BorderImage(FAppStyle::GetBrush("Menu.Background"))
				[
					SNew(SBox)
						.Visibility(EVisibility::Visible)
						.WidthOverride(500.0f)
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
								.FillHeight(1)
								[
									SNew(SBorder)
										.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
										.Content()
										[
											SAssignNew(ParentClassContainer, SVerticalBox)
										]
								]
							+ SVerticalBox::Slot()
								.AutoHeight()
								.HAlign(HAlign_Right)
								.VAlign(VAlign_Bottom)
								.Padding(8)
								[
									SNew(SUniformGridPanel)
										.SlotPadding(FAppStyle::GetMargin("StandardDialog.SlotPadding"))
										.MinDesiredSlotWidth(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
										.MinDesiredSlotHeight(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))
									+ SUniformGridPanel::Slot(0, 0)
									[
										SNew(SButton)
													.HAlign(HAlign_Center)
													.ContentPadding(FAppStyle::GetMargin("StandardDialog.ContentPadding"))
													.OnClicked(this, &SHeartInputHandlerAssetCreateDialog::OkClicked)
													.Text(LOCTEXT("CreateHeartAssetOk", "OK"))
									]
									+ SUniformGridPanel::Slot(1, 0)
									[
										SNew(SButton)
													.HAlign(HAlign_Center)
													.ContentPadding(FAppStyle::GetMargin("StandardDialog.ContentPadding"))
													.OnClicked(this, &SHeartInputHandlerAssetCreateDialog::CancelClicked)
													.Text(LOCTEXT("CreateHeartAssetCancel", "Cancel"))
									]
								]
						]
				]
		];

		MakeParentClassPicker();
	}

	/** Sets properties for the supplied UHeartInputHandlerAssetFactory */
	bool ConfigureProperties(const TWeakObjectPtr<UHeartInputHandlerAssetFactory> InHeartInputHandlerAssetFactory)
	{
		HeartInputHandlerAssetFactory = InHeartInputHandlerAssetFactory;

		const TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(LOCTEXT("HeartInputHandlerAssetCreateDialogTitle", "Pick Asset Class"))
			.ClientSize(FVector2D(400, 700))
			.SupportsMinimize(false).SupportsMaximize(false)
			[
				AsShared()
			];

		PickerWindow = Window;
		GEditor->EditorAddModalWindow(Window);

		HeartInputHandlerAssetFactory.Reset();
		return bOkClicked;
	}

private:
	/** Creates the combo menu for the parent class */
	void MakeParentClassPicker()
	{
		// Load the Class Viewer module to display a class picker
		FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

		FClassViewerInitializationOptions Options;
		Options.Mode = EClassViewerMode::ClassPicker;
		Options.DisplayMode = EClassViewerDisplayMode::TreeView;
		Options.bIsBlueprintBaseOnly = false;
		Options.bShowUnloadedBlueprints = true;

		const TSharedPtr<FHeartDefaultClassFilter> Filter = MakeShared<FHeartDefaultClassFilter>();

		Filter->AllowedChildrenOfClasses.Add(UHeartInputHandlerAssetBase::StaticClass());
		Options.ClassFilters = {Filter.ToSharedRef()};

		ParentClassContainer->ClearChildren();
		ParentClassContainer->AddSlot()
			[
				ClassViewerModule.CreateClassViewer(Options, FOnClassPicked::CreateSP(this, &SHeartInputHandlerAssetCreateDialog::OnClassPicked))
			];
	}

	/** Handler for when a parent class is selected */
	void OnClassPicked(UClass* ChosenClass)
	{
		SelectedClass = ChosenClass;
	}

	/** Handler for when ok is clicked */
	FReply OkClicked()
	{
		if (HeartInputHandlerAssetFactory.IsValid())
		{
			HeartInputHandlerAssetFactory->AssetClass = SelectedClass.Get();
		}

		CloseDialog(true);

		return FReply::Handled();
	}

	void CloseDialog(bool bWasPicked = false)
	{
		bOkClicked = bWasPicked;
		if (PickerWindow.IsValid())
		{
			PickerWindow.Pin()->RequestDestroyWindow();
		}
	}

	/** Handler for when cancel is clicked */
	FReply CancelClicked()
	{
		CloseDialog();
		return FReply::Handled();
	}

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override
	{
		if (InKeyEvent.GetKey() == EKeys::Escape)
		{
			CloseDialog();
			return FReply::Handled();
		}
		return SWidget::OnKeyDown(MyGeometry, InKeyEvent);
	}

private:
	/** The factory for which we are setting up properties */
	TWeakObjectPtr<UHeartInputHandlerAssetFactory> HeartInputHandlerAssetFactory;

	/** A pointer to the window that is asking the user to select a parent class */
	TWeakPtr<SWindow> PickerWindow;

	/** The container for the Parent Class picker */
	TSharedPtr<SVerticalBox> ParentClassContainer;

	/** The selected class */
	TWeakObjectPtr<UClass> SelectedClass;

	/** True if Ok was clicked */
	bool bOkClicked = false;
};

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

/*------------------------------------------------------------------------------
	UHeartInputHandlerAssetFactory implementation
------------------------------------------------------------------------------*/

UHeartInputHandlerAssetFactory::UHeartInputHandlerAssetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UHeartInputHandlerAssetBase::StaticClass();

	bCreateNew = true;
	bEditAfterNew = true;
}

bool UHeartInputHandlerAssetFactory::ConfigureProperties()
{
	const TSharedRef<SHeartInputHandlerAssetCreateDialog> Dialog = SNew(SHeartInputHandlerAssetCreateDialog);
	return Dialog->ConfigureProperties(this);
}

UObject* UHeartInputHandlerAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	check(Class->IsChildOf(UHeartInputHandlerAssetBase::StaticClass()));

	if (AssetClass == nullptr || !AssetClass->IsChildOf(UHeartInputHandlerAssetBase::StaticClass()))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), AssetClass ? FText::FromString(AssetClass->GetName()) : LOCTEXT("Null", "(null)"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(LOCTEXT("CannotCreateHeartInputHandlerAsset", "Cannot create a Heart Input Handler Asset based on the class '{ClassName}'."), Args));
		return nullptr;
	}

	UHeartInputHandlerAssetBase* NewAsset = NewObject<UHeartInputHandlerAssetBase>(InParent, AssetClass, Name, Flags);

	return NewAsset;
}

UObject* UHeartInputHandlerAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}

#undef LOCTEXT_NAMESPACE