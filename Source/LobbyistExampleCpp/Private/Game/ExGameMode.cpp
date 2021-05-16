// Copyright Francesco Camarlinghi. All rights reserved.

#include "Game/ExGameMode.h"
#include "Game/ExCharacter.h"
#include "Game/ExPlayerController.h"
#include "Game/ExPlayerState.h"
#include "ExCharacterSkin.h"
#include "ExGameInstance.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AExGameMode::AExGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultPawnClass = AExCharacter::StaticClass();
	PlayerStateClass = AExPlayerState::StaticClass();
	PlayerControllerClass = AExPlayerController::StaticClass();
}

void AExGameMode::ReturnToLobby()
{
	if (bReturningToLobby)
	{
		return;
	}

	if (const UWorld* World = GetWorld())
	{
		bReturningToLobby = true;

		for (auto It = World->GetPlayerControllerIterator(); It; ++It)
		{
			AExPlayerController* PlayerController = Cast<AExPlayerController>(*It);
			if (PlayerController != nullptr && !PlayerController->IsLocalController())
			{
				PlayerController->ClientTravel(TEXT("/Game/Maps/Menu"), ETravelType::TRAVEL_Absolute);
			}
		}

		// Wait a little bit for clients to receive the ReturnToLobby RPC, then return to the lobby
		World->GetTimerManager().SetTimer(
			TimerHandle_ReturnToLobbyHost, this, &ThisClass::ReturnToLobbyHost, 1.0f);
	}
}

void AExGameMode::ReturnToLobbyHost()
{
	if (GEngine != nullptr)
	{
		GEngine->SetClientTravel(GetWorld(), TEXT("/Game/Maps/Menu"), ETravelType::TRAVEL_Absolute);
	}
}

FString AExGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	const FString Error = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	if (Error.Len() > 0)
	{
		return Error;
	}

	// Init player options
	if (AExPlayerState* PlayerState = NewPlayerController->GetPlayerState<AExPlayerState>())
	{
		UExCharacterSkin* Skin = nullptr;

		const FString SkinPath = UGameplayStatics::ParseOption(Options, TEXT("CharacterSkin"));
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
			PlayerState->CharacterSkin = Skin;
		}
		else
		{
			UE_LOG(LogGameMode, Warning, TEXT("Failed to load valid skin for %s."), *PlayerState->GetHumanReadableName());
		}
	}

	return TEXT("");
}

APawn* AExGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	APawn* Pawn = Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
	if (const AExPlayerState* PlayerState = NewPlayer->GetPlayerState<AExPlayerState>())
	{
		if (AExCharacter* Character = Cast<AExCharacter>(Pawn))
		{
			Character->SetSkin(PlayerState->CharacterSkin);
		}
	}
	return Pawn;
}
