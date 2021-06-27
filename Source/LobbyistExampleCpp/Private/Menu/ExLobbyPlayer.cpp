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

void AExLobbyPlayer::SetCharacterSkin(UExCharacterSkin* NewSkin)
{
	if ((HasAuthority() || IsLocalPlayer()) && NewSkin != CharacterSkin)
	{
		CharacterSkin = NewSkin;
		OnCharacterSkinChanged();

		if (!HasAuthority())
		{
			ServerSetCharacterSkin(NewSkin);
		}
	}
}

void AExLobbyPlayer::ServerSetCharacterSkin_Implementation(UExCharacterSkin* NewSkin)
{
	SetCharacterSkin(NewSkin);
}

void AExLobbyPlayer::OnCharacterSkinChanged()
{
	// Persist selected skin to login options
	if (UExLocalPlayer* LocalPlayer = GetLocalPlayer<UExLocalPlayer>())
	{
		if (CharacterSkin != nullptr)
		{
			LocalPlayer->GameLoginOptions.Add(TEXT("CharacterSkin"), CharacterSkin->GetPathName());
		}
		else
		{
			LocalPlayer->GameLoginOptions.Remove(TEXT("CharacterSkin"));
		}
	}
}

void AExLobbyPlayer::ServerRequestKickPlayer_Implementation(AExLobbyPlayer* PlayerToKick)
{
	if (const ULobbyistSubsystem* LobbyistSubsystem = ULobbyistSubsystem::Get(GetWorld()))
	{
		if (AExLobbyHost* LobbyHost = LobbyistSubsystem->GetLobbyHost<AExLobbyHost>())
		{
			LobbyHost->ProcessKickRequest(this, PlayerToKick);
		}
	}
}

void AExLobbyPlayer::ServerSendChatMessage_Implementation(const FString& Message)
{
	if (const ULobbyistSubsystem* LobbyistSubsystem = ULobbyistSubsystem::Get(GetWorld()))
	{
		if (AExLobbyHost* LobbyHost = LobbyistSubsystem->GetLobbyHost<AExLobbyHost>())
		{
			LobbyHost->ProcessChatMessage(this, Message);
		}
	}
}

void AExLobbyPlayer::ServerStartGame_Implementation()
{
	if (const ULobbyistSubsystem* LobbyistSubsystem = ULobbyistSubsystem::Get(GetWorld()))
	{
		if (AExLobbyHost* LobbyHost = LobbyistSubsystem->GetLobbyHost<AExLobbyHost>())
		{
			LobbyHost->ProcessStartGameRequest(this);
		}
	}
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
	if (TimeStamp > LastPingTimeStamp)
	{
		ClientPingResponse(TimeStamp);
	}
}

void AExLobbyPlayer::ClientPingResponse_Implementation(const float TimeStamp)
{
	if (const UWorld* World = GetWorld())
	{
		if (TimeStamp > LastPingTimeStamp)
		{
			const float ElapsedTime = World->GetRealTimeSeconds() - TimeStamp;
			UpdatePing(ElapsedTime);
			ServerUpdatePing(TimeStamp, ElapsedTime);
			LastPingTimeStamp = TimeStamp;
		}
	}
}

void AExLobbyPlayer::UpdatePing(const float ElapsedTime)
{
	// Store exact ping on authority and owner client
	ExactPing = ElapsedTime * 1000.0f;

	// Replicate ping as uint8 to save bandwidth
	Ping = FMath::Min(FMath::TruncToInt(ExactPing * 0.25f), 255);
}

void AExLobbyPlayer::ServerUpdatePing_Implementation(const float TimeStamp, const float ElapsedTime)
{
	if (TimeStamp > LastPingTimeStamp)
	{
		UpdatePing(ElapsedTime);
		LastPingTimeStamp = TimeStamp;
	}
}

void AExLobbyPlayer::OnRep_Ping()
{
	ExactPing = static_cast<float>(Ping) * 4.0f;
}
