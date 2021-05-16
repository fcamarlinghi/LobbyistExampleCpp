// Copyright Francesco Camarlinghi. All rights reserved.

#include "UI/ExLobby.h"
#include "Menu/ExLobbyHost.h"
#include "Menu/ExLobbyPlayer.h"
#include "Menu/ExMenuPlayerController.h"
#include "UI/ExLobbyChat.h"
#include "UI/ExLobbyPlayerEntry.h"
#include "UI/ExLobbyPlayerSummary.h"
#include "LobbyistSubsystem.h"
#include "LobbyistState.h"

#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "TimerManager.h"

UExLobby::UExLobby(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UExLobby::NativeOnInitialized()
{
	if (LeaveButton != nullptr)
	{
		LeaveButton->OnClicked.AddDynamic(this, &ThisClass::OnLeaveButtonClicked);
	}
	if (StartGameButton != nullptr)
	{
		StartGameButton->OnClicked.AddDynamic(this, &ThisClass::OnStartGameButtonClicked);
	}
	Super::NativeOnInitialized();
}

void UExLobby::Activated_Implementation()
{
	Setup();
}

void UExLobby::Deactivated_Implementation()
{
	if (const UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.ClearTimer(TimerHandle_Setup);
		TimerManager.ClearTimer(TimerHandle_UpdatePlayers);
	}

	if (PlayerList != nullptr)
	{
		PlayerList->ClearListItems();
	}
	if (Chat != nullptr)
	{
		Chat->ClearChat();
		Chat->InitializeForPlayer(nullptr);
	}
	if (LocalPlayerSummary != nullptr)
	{
		LocalPlayerSummary->InitializeForPlayer(nullptr);
	}

	if (ALobbyistState* LobbyistState = WeakLobbyState.Get())
	{
		LobbyistState->OnPlayerAdded.RemoveDynamic(this, &ThisClass::OnPlayerAdded);
		LobbyistState->OnPlayerRemoved.RemoveDynamic(this, &ThisClass::OnPlayerRemoved);
	}

	WeakLobbyState.Reset();
	WeakLocalPlayer.Reset();
}

void UExLobby::Setup()
{
	if (const ULobbyistSubsystem* LobbyistSubsystem = ULobbyistSubsystem::Get(GetWorld()))
	{
		if (ALobbyistState* LobbyState = LobbyistSubsystem->GetLobbyState())
		{
			WeakLobbyState = MakeWeakObjectPtr(LobbyState);
			LobbyState->OnPlayerAdded.AddDynamic(this, &ThisClass::OnPlayerAdded);
			LobbyState->OnPlayerRemoved.AddDynamic(this, &ThisClass::OnPlayerRemoved);
		}
	}

	if (WeakLobbyState.IsValid())
	{
		UpdatePlayers();
	}
	else
	{
		// Wait for LobbyState to be replicated...
		if (const UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(TimerHandle_Setup, this, &UExLobby::Setup, 0.1f);
		}
	}
}

void UExLobby::OnPlayerAdded(ALobbyistPlayer* Player)
{
	ScheduleUpdatePlayers();
}

void UExLobby::OnPlayerRemoved(ALobbyistPlayer* Player)
{
	ScheduleUpdatePlayers();
}

void UExLobby::ScheduleUpdatePlayers()
{
	if (const UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.ClearTimer(TimerHandle_UpdatePlayers);
		TimerHandle_UpdatePlayers = TimerManager.SetTimerForNextTick(this, &UExLobby::UpdatePlayers);
	}
}

void UExLobby::UpdatePlayers()
{
	const ALobbyistState* LobbyState = WeakLobbyState.Get();
	if (LobbyState == nullptr)
	{
		WeakLocalPlayer.Reset();
		return;
	}

	TArray<UExLobbyPlayerEntryData*> PlayerEntries;
	for (ALobbyistPlayer* LobbyPlayer : LobbyState->GetPlayers())
	{
		AExLobbyPlayer* ExLobbyPlayer = Cast<AExLobbyPlayer>(LobbyPlayer);
		if (ExLobbyPlayer == nullptr)
		{
			continue;
		}

		if (ExLobbyPlayer->IsLocalPlayer())
		{
			WeakLocalPlayer = MakeWeakObjectPtr(ExLobbyPlayer);
		}

		UExLobbyPlayerEntryData* PlayerEntry = NewObject<UExLobbyPlayerEntryData>(this);
		PlayerEntry->Player = MakeWeakObjectPtr(ExLobbyPlayer);
		PlayerEntry->Index = PlayerEntries.Num();
		PlayerEntries.Add(PlayerEntry);
	}

	if (PlayerList != nullptr)
	{
		PlayerList->SetListItems(PlayerEntries);
	}
	if (PlayerCountTextBlock != nullptr)
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("Players"), FText::AsNumber(LobbyState->GetNumPlayers()));
		Args.Add(TEXT("MaxPlayers"), FText::AsNumber(GetDefault<AExLobbyHost>()->MaxPlayers));

		PlayerCountTextBlock->SetText(
			FText::Format(NSLOCTEXT("Lobby", "PlayerCount", "PLAYERS ({Players}/{MaxPlayers})"), Args));
	}
	if (PlayerListSwitcher != nullptr)
	{
		PlayerListSwitcher->SetActiveWidgetIndex(PlayerEntries.Num() > 0 ? 1 : 0);
	}

	AExLobbyPlayer* LocalPlayer = WeakLocalPlayer.Get();
	if (Chat != nullptr)
	{
		Chat->InitializeForPlayer(LocalPlayer);
	}
	if (LocalPlayerSummary != nullptr)
	{
		LocalPlayerSummary->InitializeForPlayer(LocalPlayer);
	}
	if (StartGameButton != nullptr)
	{
		if (LocalPlayer != nullptr && LocalPlayer->IsSessionOwner())
		{
			StartGameButton->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			StartGameButton->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UExLobby::OnLeaveButtonClicked()
{
	if (AExMenuPlayerController* PlayerController = GetOwningPlayer<AExMenuPlayerController>())
	{
		PlayerController->ResetSession();
	}
}

void UExLobby::OnStartGameButtonClicked()
{
	if (AExLobbyPlayer* LocalPlayer = WeakLocalPlayer.Get())
	{
		LocalPlayer->ServerStartGame();
	}
}
