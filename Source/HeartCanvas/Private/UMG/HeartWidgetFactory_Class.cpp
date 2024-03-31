// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartWidgetFactory_Class.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartWidgetFactory_Class)

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