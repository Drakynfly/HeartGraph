// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Nodes/HeartGraphNodeBlueprintFactory.h"

#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNodeBlueprint.h"

#include "Assets/HeartDefaultClassFilter.h"

#include "ClassViewerModule.h"
#include "Editor.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "SlateOptMacros.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SWindow.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNodeBlueprintFactory)

#define LOCTEXT_NAMESPACE "HeartGraphNodeBlueprintFactory"

// ------------------------------------------------------------------------------
// Dialog to configure creation properties
// ------------------------------------------------------------------------------
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

class SHeartGraphNodeBlueprintCreateDialog final : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHeartGraphNodeBlueprintCreateDialog) {}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs)
	{
		bOkClicked = false;
		ParentClass = UHeartGraphNode::StaticClass();

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
													.OnClicked(this, &SHeartGraphNodeBlueprintCreateDialog::OkClicked)
													.Text(LOCTEXT("CreateHeartGraphNodeBlueprintOk", "OK"))
									]
									+ SUniformGridPanel::Slot(1, 0)
									[
										SNew(SButton)
													.HAlign(HAlign_Center)
													.ContentPadding(FAppStyle::GetMargin("StandardDialog.ContentPadding"))
													.OnClicked(this, &SHeartGraphNodeBlueprintCreateDialog::CancelClicked)
													.Text(LOCTEXT("CreateHeartGraphNodeBlueprintCancel", "Cancel"))
									]
								]
						]
				]
		];

		MakeParentClassPicker();
	}

	/** Sets properties for the supplied HeartGraphNodeBlueprintFactory */
	bool ConfigureProperties(const TWeakObjectPtr<UHeartGraphNodeBlueprintFactory> InHeartGraphNodeBlueprintFactory)
	{
		HeartGraphNodeBlueprintFactory = InHeartGraphNodeBlueprintFactory;

		const TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(LOCTEXT("CreateHeartGraphNodeBlueprintOptions", "Pick Parent Class"))
			.ClientSize(FVector2D(400, 700))
			.SupportsMinimize(false).SupportsMaximize(false)
			[
				AsShared()
			];

		PickerWindow = Window;
		GEditor->EditorAddModalWindow(Window);

		HeartGraphNodeBlueprintFactory.Reset();
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

		// All child child classes of UHeartGraphNode are valid
		Filter->AllowedChildrenOfClasses.Add(UHeartGraphNode::StaticClass());
		Options.ClassFilters = {Filter.ToSharedRef()};

		ParentClassContainer->ClearChildren();
		ParentClassContainer->AddSlot()
			[
				ClassViewerModule.CreateClassViewer(Options, FOnClassPicked::CreateSP(this, &SHeartGraphNodeBlueprintCreateDialog::OnClassPicked))
			];
	}

	/** Handler for when a parent class is selected */
	void OnClassPicked(UClass* ChosenClass)
	{
		ParentClass = ChosenClass;
	}

	/** Handler for when ok is clicked */
	FReply OkClicked()
	{
		if (HeartGraphNodeBlueprintFactory.IsValid())
		{
			HeartGraphNodeBlueprintFactory->ParentClass = ParentClass.Get();
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
	TWeakObjectPtr<UHeartGraphNodeBlueprintFactory> HeartGraphNodeBlueprintFactory;

	/** A pointer to the window that is asking the user to select a parent class */
	TWeakPtr<SWindow> PickerWindow;

	/** The container for the Parent Class picker */
	TSharedPtr<SVerticalBox> ParentClassContainer;

	/** The selected class */
	TWeakObjectPtr<UClass> ParentClass;

	/** True if Ok was clicked */
	bool bOkClicked = false;
};

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

/*------------------------------------------------------------------------------
	UHeartGraphNodeBlueprintFactory implementation
------------------------------------------------------------------------------*/

UHeartGraphNodeBlueprintFactory::UHeartGraphNodeBlueprintFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UHeartGraphNodeBlueprint::StaticClass();
	ParentClass = UHeartGraphNode::StaticClass();

	bCreateNew = true;
	bEditAfterNew = true;
}

bool UHeartGraphNodeBlueprintFactory::ConfigureProperties()
{
	const TSharedRef<SHeartGraphNodeBlueprintCreateDialog> Dialog = SNew(SHeartGraphNodeBlueprintCreateDialog);
	return Dialog->ConfigureProperties(this);
}

UObject* UHeartGraphNodeBlueprintFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	check(Class->IsChildOf(UHeartGraphNodeBlueprint::StaticClass()));

	if (ParentClass == nullptr ||
		!FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass) ||
		!ParentClass->IsChildOf(UHeartGraphNode::StaticClass()))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), ParentClass ? FText::FromString(ParentClass->GetName()) : LOCTEXT("Null", "(null)"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(LOCTEXT("CannotCreateHeartGraphNodeBlueprint", "Cannot create a Heart Graph Node Blueprint based on the class '{ClassName}'."), Args));
		return nullptr;
	}

	UHeartGraphNodeBlueprint* NewBP = CastChecked<UHeartGraphNodeBlueprint>(
		FKismetEditorUtilities::CreateBlueprint(
			ParentClass, InParent, Name, BPTYPE_Normal,
			UHeartGraphNodeBlueprint::StaticClass(),
			UBlueprintGeneratedClass::StaticClass(),
			CallingContext));

	return NewBP;
}

UObject* UHeartGraphNodeBlueprintFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}

#undef LOCTEXT_NAMESPACE