// Copyright Francesco Camarlinghi. All rights reserved.

#include "UI/ExLobbyPlayerEntry.h"
#include "Menu/ExLobbyPlayer.h"
#include "ExCharacterSkin.h"
#include "LobbyistState.h"
#include "LobbyistSubsystem.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

UExLobbyPlayerEntry::UExLobbyPlayerEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UExLobbyPlayerEntry::NativeOnInitialized()
{
	if (SkinImage != nullptr)
	{
		SkinImage->ColorAndOpacityDelegate.BindDynamic(this, &ThisClass::GetSkinColor);
	}
	if (PingTextBlock != nullptr)
	{
		PingTextBlock->TextDelegate.BindDynamic(this, &ThisClass::GetPingText);
	}
	if (KickButton != nullptr)
	{
		KickButton->OnClicked.AddDynamic(this, &ThisClass::OnKickButtonClicked);
	}
	Super::NativeOnInitialized();
}

void UExLobbyPlayerEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UExLobbyPlayerEntryData* EntryData = CastChecked<UExLobbyPlayerEntryData>(ListItemObject);
	WeakEntryData = MakeWeakObjectPtr(EntryData);

	if (KickButton != nullptr)
	{
		KickButton->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (const AExLobbyPlayer* Player = EntryData->Player.Get())
	{
		if (PlayerNameTextBlock != nullptr)
		{
			if (Player->IsLocalPlayer())
			{
				PlayerNameTextBlock->SetText(NSLOCTEXT("Lobby", "PlayerNameYou", "You"));
			}
			else
			{
				PlayerNameTextBlock->SetText(FText::AsCultureInvariant(Player->GetPlayerName()));
			}
		}

		if (LeaderBox != nullptr)
		{
			LeaderBox->SetVisibility(
				Player->IsSessionOwner() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		}

		if (Player->IsLocalPlayer())
		{
			if (LeaderBox != nullptr)
			{
				LeaderBox->SetVisibility(ESlateVisibility::Collapsed);
			}
			if (SeparatorImage != nullptr)
			{
				SeparatorImage->SetVisibility(ESlateVisibility::Collapsed);
			}
			if (PingTextBlock != nullptr)
			{
				PingTextBlock->SetVisibility(ESlateVisibility::Collapsed);
			}
		}

		// Show the kick button if the local client owns the lobby
		const ULobbyistSubsystem* LobbyistSubsystem = ULobbyistSubsystem::Get(GetWorld());
		const ALobbyistState* LobbyistState = (LobbyistSubsystem != nullptr) ? LobbyistSubsystem->GetLobbyState() : nullptr;
		if (LobbyistState != nullptr)
		{
			if (AExLobbyPlayer* LobbyOwner = Cast<AExLobbyPlayer>(LobbyistState->GetSessionOwner()))
			{
				WeakLobbyOwner = MakeWeakObjectPtr(LobbyOwner);
				if (LobbyOwner->IsLocalPlayer()
					&& !Player->IsLocalPlayer()
					&& KickButton != nullptr)
				{
					KickButton->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
	}

	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
}

FText UExLobbyPlayerEntry::GetPingText()
{
	if (const UExLobbyPlayerEntryData* EntryData = WeakEntryData.Get())
	{
		if (const AExLobbyPlayer* Player = EntryData->Player.Get())
		{
			if (Player->IsSessionHost())
			{
				return NSLOCTEXT("Lobby", "PingHost", "Host");
			}

			FFormatNamedArguments Args;
			Args.Add(TEXT("Ping"), FText::AsNumber(Player->GetPing()));
			return FText::Format(NSLOCTEXT("Lobby", "PingFormat", "Ping {Ping}"), Args);
		}
	}
	return NSLOCTEXT("Lobby", "PingMax", "Ping 255");
}

FLinearColor UExLobbyPlayerEntry::GetSkinColor()
{
	if (const UExLobbyPlayerEntryData* EntryData = WeakEntryData.Get())
	{
		const AExLobbyPlayer* Player = EntryData->Player.Get();
		if (Player != nullptr && Player->GetCharacterSkin() != nullptr)
		{
			return Player->GetCharacterSkin()->Color;
		}
	}
	return FLinearColor::White;
}

void UExLobbyPlayerEntry::OnKickButtonClicked()
{
	if (AExLobbyPlayer* LobbyOwner = WeakLobbyOwner.Get())
	{
		const UExLobbyPlayerEntryData* EntryData = WeakEntryData.Get();
		if (EntryData != nullptr && EntryData->Player.IsValid())
		{
			LobbyOwner->ServerRequestKickPlayer(EntryData->Player.Get());
		}
	}
}
