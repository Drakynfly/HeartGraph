// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartGraphBlueprintFactory.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphBlueprint.h"

#include "Assets/HeartDefaultClassFilter.h"

#include "ClassViewerModule.h"
#include "Editor.h"
#include "Kismet2/KismetEditorUtilities.h"
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

#define LOCTEXT_NAMESPACE "HeartGraphBlueprintFactory"

// ------------------------------------------------------------------------------
// Dialog to configure creation properties
// ------------------------------------------------------------------------------
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

class SHeartGraphBlueprintCreateDialog final : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHeartGraphBlueprintCreateDialog) {}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs)
	{
		bOkClicked = false;
		SelectedClass = UHeartGraph::StaticClass();

		ChildSlot
		[
			SNew(SBorder)
				.Visibility(EVisibility::Visible)
				.BorderImage(FAppStyle::Get().GetBrush("Menu.Background"))
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
										.BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
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
										.SlotPadding(FAppStyle::Get().GetMargin("StandardDialog.SlotPadding"))
										.MinDesiredSlotWidth(FAppStyle::Get().GetFloat("StandardDialog.MinDesiredSlotWidth"))
										.MinDesiredSlotHeight(FAppStyle::Get().GetFloat("StandardDialog.MinDesiredSlotHeight"))
										+ SUniformGridPanel::Slot(0, 0)
											[
												SNew(SButton)
													.HAlign(HAlign_Center)
													.ContentPadding(FAppStyle::Get().GetMargin("StandardDialog.ContentPadding"))
													.OnClicked(this, &SHeartGraphBlueprintCreateDialog::OkClicked)
													.Text(LOCTEXT("CreateHeartGraphBlueprintOk", "OK"))
											]
										+ SUniformGridPanel::Slot(1, 0)
											[
												SNew(SButton)
													.HAlign(HAlign_Center)
													.ContentPadding(FAppStyle::Get().GetMargin("StandardDialog.ContentPadding"))
													.OnClicked(this, &SHeartGraphBlueprintCreateDialog::CancelClicked)
													.Text(LOCTEXT("CreateHeartGraphBlueprintCancel", "Cancel"))
											]
								]
						]
				]
		];

		MakeParentClassPicker();
	}

	/** Sets properties for the supplied HeartGraphBlueprintFactory */
	bool ConfigureProperties(const TWeakObjectPtr<UHeartGraphBlueprintFactory> InHeartGraphBlueprintFactory)
	{
		HeartGraphBlueprintFactory = InHeartGraphBlueprintFactory;

		const TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(LOCTEXT("CreateHeartGraphBlueprintOptions", "Pick Parent Class"))
			.ClientSize(FVector2D(400, 700))
			.SupportsMinimize(false).SupportsMaximize(false)
			[
				AsShared()
			];

		PickerWindow = Window;
		GEditor->EditorAddModalWindow(Window);

		HeartGraphBlueprintFactory.Reset();
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
		Options.bIsBlueprintBaseOnly = true;

		const TSharedPtr<FHeartDefaultClassFilter> Filter = MakeShareable(new FHeartDefaultClassFilter);

		// All child child classes of UHeartGraph are valid
		Filter->AllowedChildrenOfClasses.Add(UHeartGraph::StaticClass());
		Options.ClassFilters = {Filter.ToSharedRef()};

		ParentClassContainer->ClearChildren();
		ParentClassContainer->AddSlot()
			[
				ClassViewerModule.CreateClassViewer(Options, FOnClassPicked::CreateSP(this, &SHeartGraphBlueprintCreateDialog::OnClassPicked))
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
		if (HeartGraphBlueprintFactory.IsValid())
		{
			HeartGraphBlueprintFactory->ParentClass = SelectedClass.Get();
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
	TWeakObjectPtr<UHeartGraphBlueprintFactory> HeartGraphBlueprintFactory;

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
	UHeartGraphBlueprintFactory implementation
------------------------------------------------------------------------------*/

UHeartGraphBlueprintFactory::UHeartGraphBlueprintFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UHeartGraphBlueprint::StaticClass();
	ParentClass = UHeartGraph::StaticClass();

	bCreateNew = true;
	bEditAfterNew = true;
}

bool UHeartGraphBlueprintFactory::ConfigureProperties()
{
	const TSharedRef<SHeartGraphBlueprintCreateDialog> Dialog = SNew(SHeartGraphBlueprintCreateDialog);
	return Dialog->ConfigureProperties(this);
}

UObject* UHeartGraphBlueprintFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	check(Class->IsChildOf(UHeartGraphBlueprint::StaticClass()));

	if (ParentClass == nullptr || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass) || !ParentClass->IsChildOf(UHeartGraph::StaticClass()))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), ParentClass ? FText::FromString(ParentClass->GetName()) : LOCTEXT("Null", "(null)"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(LOCTEXT("CannotCreateHeartGraphBlueprint", "Cannot create a Heart Graph Blueprint based on the class '{ClassName}'."), Args));
		return nullptr;
	}

	UHeartGraphBlueprint* NewBP = CastChecked<UHeartGraphBlueprint>(FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, Name, BPTYPE_Normal, UHeartGraphBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), CallingContext));

	if (NewBP && NewBP->UbergraphPages.Num() > 0)
	{
		//UBlueprintEditorSettings* Settings = GetMutableDefault<UBlueprintEditorSettings>();
		//if(Settings && Settings->bSpawnDefaultBlueprintNodes)
		{
			int32 NodePositionY = 0;
			//FKismetEditorUtilities::AddDefaultEventNode(NewBP, NewBP->UbergraphPages[0], FName("K2_ExecuteInput"), UHeartGraphNode::StaticClass(), NodePositionY);
			//FKismetEditorUtilities::AddDefaultEventNode(NewBP, NewBP->UbergraphPages[0], FName("K2_Cleanup"), UHeartGraphNode::StaticClass(), NodePositionY);
		}
	}

	return NewBP;
}

UObject* UHeartGraphBlueprintFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}

#undef LOCTEXT_NAMESPACE
