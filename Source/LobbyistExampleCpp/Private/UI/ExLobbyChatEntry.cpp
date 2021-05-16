// Copyright Francesco Camarlinghi. All rights reserved.

#include "UI/ExLobbyChatEntry.h"

#include "Components/TextBlock.h"

UExLobbyChatEntry::UExLobbyChatEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UExLobbyChatEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UExLobbyChatEntryData* EntryData = CastChecked<UExLobbyChatEntryData>(ListItemObject);
	WeakEntryData = MakeWeakObjectPtr(EntryData);

	if (MessageTextBlock != nullptr)
	{
		MessageTextBlock->SetText(EntryData->Message);
	}

	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
}
