// Copyright Francesco Camarlinghi. All rights reserved.

#include "UI/ExLobbyPlayerEntry.h"
#include "Menu/ExLobbyPlayer.h"
#include "ExCharacterSkin.h"
#include "LobbyistState.h"
#include "LobbyistSubsystem.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#define LOCTEXT_NAMESPACE "LobbyPlayerEntry"

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
		KickButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	Super::NativeOnInitialized();
}

void UExLobbyPlayerEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UExLobbyPlayerEntryData* EntryData = CastChecked<UExLobbyPlayerEntryData>(ListItemObject);
	WeakEntryData = MakeWeakObjectPtr(EntryData);

	if (EntryData->Player != nullptr)
	{
		if (PlayerNameTextBlock != nullptr)
		{
			if (EntryData->Player->IsLocalPlayer())
			{
				PlayerNameTextBlock->SetText(LOCTEXT("PlayerNameYou", "You"));
			}
			else
			{
				PlayerNameTextBlock->SetText(FText::AsCultureInvariant(EntryData->Player->GetPlayerName()));
			}
		}

		if (LeaderBox != nullptr)
		{
			LeaderBox->SetVisibility(
				EntryData->Player->IsSessionOwner() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		}

		if (!EntryData->Player->IsLocalPlayer())
		{
			if (LeaderBox != nullptr)
			{
				LeaderBox->SetVisibility(ESlateVisibility::Collapsed);
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
					&& !EntryData->Player->IsLocalPlayer()
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
	const UExLobbyPlayerEntryData* EntryData = WeakEntryData.Get();
	if (EntryData != nullptr && EntryData->Player != nullptr)
	{
		if (EntryData->Player->IsSessionHost())
		{
			return LOCTEXT("PingHost", "Host");
		}

		FFormatNamedArguments Args;
		Args.Add(TEXT("Ping"), FText::AsNumber(EntryData->Player->GetPing()));
		return FText::Format(LOCTEXT("PingFormat", "Ping {Ping}"), Args);
	}
	return LOCTEXT("PingMax", "Ping 255");
}

FLinearColor UExLobbyPlayerEntry::GetSkinColor()
{
	const UExLobbyPlayerEntryData* EntryData = WeakEntryData.Get();
	if (EntryData != nullptr
		&& EntryData->Player != nullptr
		&& EntryData->Player->GetCharacterSkin() != nullptr)
	{
		return EntryData->Player->GetCharacterSkin()->Color;
	}
	return FLinearColor::White;
}

void UExLobbyPlayerEntry::OnKickButtonClicked()
{
	if (AExLobbyPlayer* LobbyOwner = WeakLobbyOwner.Get())
	{
		const UExLobbyPlayerEntryData* EntryData = WeakEntryData.Get();
		if (EntryData != nullptr && EntryData->Player != nullptr)
		{
			LobbyOwner->ServerRequestKickPlayer(EntryData->Player);
		}
	}
}

#undef LOCTEXT_NAMESPACE
