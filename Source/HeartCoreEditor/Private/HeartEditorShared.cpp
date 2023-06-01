// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartEditorShared.h"

#include "HeartCoreEditorModule.h"
#include "SSettingsEditorCheckoutNotice.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "Heart::EditorShared"

namespace Heart::EditorShared
{
	bool CheckOutFile(const FString& FileName, const bool ShowNotification)
	{
		bool bSuccess;
		FText Result;

		if (!SettingsHelpers::IsCheckedOut(FileName, true))
		{
			FText ErrorMessage;
			bSuccess = SettingsHelpers::CheckOutOrAddFile(FileName, true, !IsRunningCommandlet(), &ErrorMessage);
			if (bSuccess)
			{
				Result = LOCTEXT("CheckedOutFile", "Checked out {0}");
			}
			else
			{
				UE_LOG(LogHeartCoreEditor, Error, TEXT("%s"), *ErrorMessage.ToString());
				bSuccess = SettingsHelpers::MakeWritable(FileName);

				if (bSuccess)
				{
					Result = LOCTEXT("MadeFileWritable", "Made {0} writable (you may need to manually add to revision control)");
				}
				else
				{
					Result = LOCTEXT("FailedToTouchFile", "Failed to check out {0} or make it writable, so no rule was added");
				}
			}
		}
		else
		{
			Result = LOCTEXT("UpdatedFile", "Updated {0}");
			bSuccess = true;
		}

		if (ShowNotification)
		{
			// Show a message that the file was checked out/updated and must be submitted
			FNotificationInfo Info(FText::Format(Result, FText::FromString(FPaths::GetCleanFilename(FileName))));
			Info.ExpireDuration = 3.0f;
			FSlateNotificationManager::Get().AddNotification(Info);
		}

		return bSuccess;
	}
}

#undef LOCTEXT_NAMESPACE