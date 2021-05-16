// Copyright Francesco Camarlinghi. All rights reserved.

#include "UI/ExLobbyPlayerSummary.h"
#include "Menu/ExLobbyPlayer.h"
#include "ExCharacterSkin.h"
#include "ExGameInstance.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

UExLobbyPlayerSummary::UExLobbyPlayerSummary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UExLobbyPlayerSummary::NativeOnInitialized()
{
	if (SkinImage != nullptr)
	{
		SkinImage->ColorAndOpacityDelegate.BindDynamic(this, &ThisClass::GetSkinColor);
	}
	if (PreviousSkinButton != nullptr)
	{
		PreviousSkinButton->OnClicked.AddDynamic(this, &ThisClass::OnPreviousSkinButtonClicked);
	}
	if (SkinNameTextBlock != nullptr)
	{
		SkinNameTextBlock->TextDelegate.BindDynamic(this, &ThisClass::GetSkinName);
	}
	if (NextSkinButton != nullptr)
	{
		NextSkinButton->OnClicked.AddDynamic(this, &ThisClass::OnNextSkinButtonClicked);
	}
	if (PingTextBlock != nullptr)
	{
		PingTextBlock->TextDelegate.BindDynamic(this, &ThisClass::GetPingText);
	}
	Super::NativeOnInitialized();
}

void UExLobbyPlayerSummary::InitializeForPlayer(AExLobbyPlayer* NewPlayer)
{
	WeakPlayer = MakeWeakObjectPtr(NewPlayer);

	if (NewPlayer == nullptr)
	{
		if (SummaryBox != nullptr)
		{
			SummaryBox->SetVisibility(ESlateVisibility::Hidden);
		}
		return;
	}

	if (SummaryBox != nullptr)
	{
		SummaryBox->SetVisibility(ESlateVisibility::Visible);
	}
	if (PlayerNameTextBlock != nullptr)
	{
		PlayerNameTextBlock->SetText(FText::AsCultureInvariant(NewPlayer->GetPlayerName()));
	}
	if (LeaderBox != nullptr)
	{
		LeaderBox->SetVisibility(NewPlayer->IsSessionOwner() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

FText UExLobbyPlayerSummary::GetPingText()
{
	if (AExLobbyPlayer* Player = WeakPlayer.Get())
	{
		if (Player->IsSessionHost())
		{
			return NSLOCTEXT("Lobby", "PingHost", "Host");
		}

		FFormatNamedArguments Args;
		Args.Add(TEXT("Ping"), FText::AsNumber(Player->GetPing()));
		return FText::Format(NSLOCTEXT("Lobby", "PingFormat", "Ping {Ping}"), Args);
	}
	return NSLOCTEXT("Lobby", "PingMax", "Ping 255");
}

FText UExLobbyPlayerSummary::GetSkinName()
{
	AExLobbyPlayer* Player = WeakPlayer.Get();
	if (Player != nullptr && Player->GetCharacterSkin() != nullptr)
	{
		return Player->GetCharacterSkin()->Name;
	}
	return FText::GetEmpty();
}

FLinearColor UExLobbyPlayerSummary::GetSkinColor()
{
	AExLobbyPlayer* Player = WeakPlayer.Get();
	if (Player != nullptr && Player->GetCharacterSkin() != nullptr)
	{
		return Player->GetCharacterSkin()->Color;
	}
	return FLinearColor::White;
}

void UExLobbyPlayerSummary::OnPreviousSkinButtonClicked()
{
	AExLobbyPlayer* Player = WeakPlayer.Get();
	const UExGameInstance* GameInstance = GetGameInstance<UExGameInstance>();
	if (Player == nullptr || GameInstance == nullptr || GameInstance->AvailableSkins.Num() == 0)
	{
		return;
	}

	const TSoftObjectPtr<UExCharacterSkin> SelectedSkin = Player->GetCharacterSkin();
	int32 PrevSkinIndex = GameInstance->AvailableSkins.IndexOfByKey(SelectedSkin) - 1;
	if (PrevSkinIndex < 0)
	{
		PrevSkinIndex = GameInstance->AvailableSkins.Num() - 1;
	}

	Player->SetCharacterSkin(GameInstance->AvailableSkins[PrevSkinIndex].LoadSynchronous());
}

void UExLobbyPlayerSummary::OnNextSkinButtonClicked()
{
	AExLobbyPlayer* Player = WeakPlayer.Get();
	const UExGameInstance* GameInstance = GetGameInstance<UExGameInstance>();
	if (Player == nullptr || GameInstance == nullptr || GameInstance->AvailableSkins.Num() == 0)
	{
		return;
	}

	const TSoftObjectPtr<UExCharacterSkin> SelectedSkin = Player->GetCharacterSkin();
	const int32 NextSkinIndex =
		(GameInstance->AvailableSkins.IndexOfByKey(SelectedSkin) + 1) % GameInstance->AvailableSkins.Num();

	Player->SetCharacterSkin(GameInstance->AvailableSkins[NextSkinIndex].LoadSynchronous());
}
