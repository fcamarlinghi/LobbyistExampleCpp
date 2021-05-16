// Copyright Francesco Camarlinghi. All rights reserved.

#include "UI/ExLobbyBrowserEntry.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

UExLobbyBrowserEntry::UExLobbyBrowserEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UExLobbyBrowserEntry::NativeOnInitialized()
{
	if (JoinButton != nullptr)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClicked);
	}
	Super::NativeOnInitialized();
}

void UExLobbyBrowserEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UExLobbyBrowserEntryData* EntryData = CastChecked<UExLobbyBrowserEntryData>(ListItemObject);
	WeakEntryData = MakeWeakObjectPtr(EntryData);

	if (HostNameTextBlock != nullptr)
	{
		const FString& HostName = EntryData->SearchResult.Session.OwningUserName;
		if (HostName.Len() > 16)
		{
			HostNameTextBlock->SetText(FText::AsCultureInvariant(HostName.Left(16) + TEXT("\x2026")));
		}
		else
		{
			HostNameTextBlock->SetText(FText::AsCultureInvariant(HostName));
		}
	}

	if (PlayerCountTextBlock != nullptr)
	{
		const FOnlineSession& Session = EntryData->SearchResult.Session;
		const int32 CurrentPlayers = Session.SessionSettings.NumPublicConnections - Session.NumOpenPublicConnections;
		const int32 MaxPlayers = Session.SessionSettings.NumPublicConnections;

		FFormatNamedArguments Args;
		Args.Add(TEXT("CurrentPlayers"), FText::AsNumber(CurrentPlayers));
		Args.Add(TEXT("MaxPlayers"), FText::AsNumber(MaxPlayers));
		PlayerCountTextBlock->SetText(FText::Format(INVTEXT("{CurrentPlayers}/{MaxPlayers}"), Args));
	}

	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
}

void UExLobbyBrowserEntry::OnJoinButtonClicked()
{
	if (UExLobbyBrowserEntryData* EntryData = WeakEntryData.Get())
	{
		EntryData->OnJoinSession.ExecuteIfBound(EntryData);
	}
}
