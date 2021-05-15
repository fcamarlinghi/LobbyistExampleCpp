// Copyright Francesco Camarlinghi. All rights reserved.

#include "Menu/ExLobbyHost.h"
#include "Menu/ExLobbyClient.h"
#include "ExGameInstance.h"
#include "ExCharacterSkin.h"
#include "LobbyistClient.h"
#include "LobbyistGlobals.h"
#include "LobbyistState.h"

#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

AExLobbyHost::AExLobbyHost(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void AExLobbyHost::ProcessKickRequest(AExLobbyPlayer* KickInstigator, AExLobbyPlayer* PlayerToKick)
{
	// Only the lobby leader can kick other players
	if (KickInstigator != nullptr
		&& KickInstigator->IsSessionOwner()
		&& PlayerToKick != nullptr)
	{
		DisconnectClient(
			PlayerToKick->GetLobbyClient(),
			NSLOCTEXT("Lobby", "PlayerKicked", "The lobby owner has decided you needed to leave.")
		);
	}
}

void AExLobbyHost::ProcessChatMessage(AExLobbyPlayer* Sender, const FString& Message)
{
	if (Sender != nullptr && Message.Len() > 0)
	{
		const FString FormattedMessage = FString::Printf(TEXT("%s: %s"), *Sender->GetPlayerName(), *Message);

		for (ALobbyistClient* LobbyClient : GetLobbyClients())
		{
			if (AExLobbyClient* ExLobbyClient = Cast<AExLobbyClient>(LobbyClient))
			{
				ExLobbyClient->ClientReceiveChatMessage(FormattedMessage);
			}
		}
	}
}

void AExLobbyHost::ProcessStartGameRequest(AExLobbyPlayer* Sender)
{
	if (!bStarting && Sender != nullptr && Sender->IsSessionOwner())
	{
		bStarting = true;
		StartGame();
	}
}

FLobbyistLoginResult AExLobbyHost::ApproveLogin_Implementation(const FUniqueNetIdRepl& InUniqueId, const FString& InOptions)
{
	if (bStarting)
	{
		return FLobbyistLoginResult::Failure(NSLOCTEXT("Lobby", "LoginTooLate", "Game is already starting."));
	}

	if (GetLobbyState() == nullptr || GetLobbyState()->GetNumPlayers() >= MaxPlayers)
	{
		return FLobbyistLoginResult::Failure(NSLOCTEXT("Lobby", "LoginFull", "Game is now full."));
	}

	return FLobbyistLoginResult::Success();
}

bool AExLobbyHost::InitNewPlayer(ALobbyistPlayer* NewPlayer, const FString& InOptions)
{
	if (!Super::InitNewPlayer(NewPlayer, InOptions))
	{
		return false;
	}

	// Clamp player name to 16 chars max
	if (NewPlayer->GetPlayerName().Len() > 16)
	{
		ChangePlayerName(NewPlayer, NewPlayer->GetPlayerName().Left(16) + TEXT("\x2026"));
	}

	// Init player options
	if (AExLobbyPlayer* LobbyPlayer = Cast<AExLobbyPlayer>(NewPlayer))
	{
		UExCharacterSkin* Skin = nullptr;

		const FString SkinPath = UGameplayStatics::ParseOption(InOptions, TEXT("CharacterSkin"));
		if (SkinPath.Len() > 0)
		{
			Skin = LoadObject<UExCharacterSkin>(nullptr, *SkinPath);
		}

		if (Skin == nullptr)
		{
			const UExGameInstance* GameInstance = GetGameInstance<UExGameInstance>();
			if (GameInstance != nullptr && GameInstance->AvailableSkins.Num() > 0)
			{
				const int32 RandomSkinIndex = FMath::RandHelper(GameInstance->AvailableSkins.Num());
				Skin = GameInstance->AvailableSkins[RandomSkinIndex].LoadSynchronous();
			}
		}

		if (Skin != nullptr)
		{
			LobbyPlayer->SetCharacterSkin(Skin);
		}
		else
		{
			UE_LOG(LogLobbyist, Warning, TEXT("Failed to load valid skin for %s."), *NewPlayer->GetPlayerName());
		}
	}

	// Notify clients that a new player has joined the lobby
	for (ALobbyistClient* Client : GetLobbyClients())
	{
		if (AExLobbyClient* LobbyClient = Cast<AExLobbyClient>(Client))
		{
			LobbyClient->ClientReceiveJoinMessage(NewPlayer->GetPlayerName());
		}
	}

	return true;
}

void AExLobbyHost::Logout(ALobbyistPlayer* ExitingPlayer)
{
	Super::Logout(ExitingPlayer);

	// Notify clients that a player is leaving the lobby
	for (ALobbyistClient* Client : GetLobbyClients())
	{
		if (AExLobbyClient* LobbyClient = Cast<AExLobbyClient>(Client))
		{
			LobbyClient->ClientReceiveLeaveMessage(ExitingPlayer->GetPlayerName());
		}
	}
}

void AExLobbyHost::TravelToGame()
{
	Super::TravelToGame();

	if (GEngine != nullptr)
	{
		GEngine->SetClientTravel(
			GetWorld(),
			TEXT("/Game/Maps/ThirdPersonExampleMap?Listen"),
			ETravelType::TRAVEL_Absolute
		);
	}
}
