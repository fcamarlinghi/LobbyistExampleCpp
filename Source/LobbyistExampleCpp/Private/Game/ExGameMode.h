// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ExGameMode.generated.h"

UCLASS(Abstract)
class AExGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AExGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	// Handles
	FTimerHandle TimerHandle_ReturnToLobbyHost;

public:

	/** Returns to the lobby. */
	void ReturnToLobby();

protected:

	/** Called after all clients have been told to return to the lobby. */
	void ReturnToLobbyHost();

	// AGameModeBase interface
public:
	FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
	APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
};
