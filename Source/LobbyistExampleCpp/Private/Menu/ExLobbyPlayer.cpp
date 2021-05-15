// Copyright Francesco Camarlinghi. All rights reserved.

#include "Menu/ExLobbyPlayer.h"
#include "Menu/ExLobbyHost.h"
#include "ExCharacterSkin.h"
#include "ExLocalPlayer.h"
#include "LobbyistSubsystem.h"

#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

AExLobbyPlayer::AExLobbyPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void AExLobbyPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Kickstart ping calculation on remote clients
	if (IsNetMode(ENetMode::NM_Client))
	{
		RequestPingUpdate();
	}
}

void AExLobbyPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CharacterSkin);
	DOREPLIFETIME_CONDITION(ThisClass, Ping, COND_SkipOwner);
}

int32 AExLobbyPlayer::GetPing() const
{
	if (HasAuthority() || IsLocalPlayer())
	{
		// Use exact ping on owner client and authority
		return FMath::TruncToInt(ExactPing);
	}

	// Use approximated replicated ping on remote clients
	return static_cast<int32>(Ping) * 4;
}

void AExLobbyPlayer::SetCharacterSkin(UExCharacterSkin* NewSkin)
{
	if (HasAuthority() || IsLocalPlayer())
	{
		CharacterSkin = NewSkin;

		// Persist selected skin to login options
		if (UExLocalPlayer* LocalPlayer = Cast<UExLocalPlayer>(GetLocalPlayer()))
		{
			if (NewSkin != nullptr)
			{
				LocalPlayer->GameLoginOptions.Add(TEXT("CharacterSkin"), NewSkin->GetPathName());
			}
			else
			{
				LocalPlayer->GameLoginOptions.Remove(TEXT("CharacterSkin"));
			}
		}

		if (!HasAuthority())
		{
			ServerSetCharacterSkin(NewSkin);
		}
	}
}

void AExLobbyPlayer::ServerRequestKickPlayer_Implementation(AExLobbyPlayer* PlayerToKick)
{
	if (const ULobbyistSubsystem* LobbyistSubsystem = ULobbyistSubsystem::Get(GetWorld()))
	{
		if (AExLobbyHost* LobbyHost = Cast<AExLobbyHost>(LobbyistSubsystem->GetLobbyHost()))
		{
			LobbyHost->ProcessKickRequest(this, PlayerToKick);
		}
	}
}

void AExLobbyPlayer::ServerSendChatMessage_Implementation(const FString& Message)
{
	if (const ULobbyistSubsystem* LobbyistSubsystem = ULobbyistSubsystem::Get(GetWorld()))
	{
		if (AExLobbyHost* LobbyHost = Cast<AExLobbyHost>(LobbyistSubsystem->GetLobbyHost()))
		{
			LobbyHost->ProcessChatMessage(this, Message);
		}
	}
}

void AExLobbyPlayer::ServerStartGame_Implementation()
{
	if (const ULobbyistSubsystem* LobbyistSubsystem = ULobbyistSubsystem::Get(GetWorld()))
	{
		if (AExLobbyHost* LobbyHost = Cast<AExLobbyHost>(LobbyistSubsystem->GetLobbyHost()))
		{
			LobbyHost->ProcessStartGameRequest(this);
		}
	}
}

void AExLobbyPlayer::ServerSetCharacterSkin_Implementation(UExCharacterSkin* NewSkin)
{
	SetCharacterSkin(NewSkin);
}

void AExLobbyPlayer::RequestPingUpdate()
{
	if (const UWorld* World = GetWorld())
	{
		ServerPingRequest(World->GetRealTimeSeconds());

		World->GetTimerManager().SetTimer(
			TimerHandle_RequestPingUpdate, this, &ThisClass::RequestPingUpdate, 0.5f);
	}
}

void AExLobbyPlayer::ServerPingRequest_Implementation(const float TimeStamp)
{
	ClientPingResponse(TimeStamp);
}

void AExLobbyPlayer::ClientPingResponse_Implementation(const float TimeStamp)
{
	if (const UWorld* World = GetWorld())
	{
		UpdatePing(World->GetRealTimeSeconds() - TimeStamp);
	}
}

void AExLobbyPlayer::UpdatePing(const float ElapsedTime)
{
	// Store exact ping on authority and owner client
	ExactPing = ElapsedTime * 1000.0f;

	// Replicate ping as uint8 to save bandwidth
	Ping = FMath::Min(FMath::TruncToInt(ExactPing * 0.25f), 255);

	if (!HasAuthority())
	{
		ServerUpdatePing(ElapsedTime);
	}
}

void AExLobbyPlayer::ServerUpdatePing_Implementation(const float ElapsedTime)
{
	UpdatePing(ElapsedTime);
}
