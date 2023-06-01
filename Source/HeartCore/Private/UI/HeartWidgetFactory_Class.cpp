// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetFactory_Class.h"

TSubclassOf<UUserWidget> UHeartWidgetFactory_Class::FindWidgetClassForData_Implementation(const UObject* Data) const
{
	if (auto&& StartingClass = Cast<UClass>(Data))
	{
		// Starting with the current class, work backwards to see if there are any construction rules for this class.
		for (const UClass* Class = StartingClass; Class; Class = Class->GetSuperClass())
		{
			TSoftClassPtr<UObject> ClassPtr(Class);
			if (const TSubclassOf<UUserWidget> EntryWidgetClassPtr = EntryWidgetForClass.FindRef(ClassPtr))
			{
				return EntryWidgetClassPtr;
			}
		}
	}

	return TSubclassOf<UUserWidget>();
}