// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"

#include "ExLobbyPlayer.h"
#include "LobbyistHost.h"
#include "ExLobbyHost.generated.h"

UCLASS()
class AExLobbyHost : public ALobbyistHost
{
	GENERATED_BODY()

public:

	AExLobbyHost(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Maximum number o8f players allowed in the game. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lobby Host")
	int32 MaxPlayers = 0;

protected:

	/** Whether the game is starting. */
	bool bStarting = false;

public:

	/** Process a request to kick a player. */
	void ProcessKickRequest(AExLobbyPlayer* KickInstigator, AExLobbyPlayer* PlayerToKick);

	/** Process a chat message, replicated it to all clients. */
	void ProcessChatMessage(AExLobbyPlayer* Sender, const FString& Message);

	/** Process a request to start the game. */
	void ProcessStartGameRequest(AExLobbyPlayer* Sender);

	// ALobbyistHost interface
protected:
	FLobbyistLoginResult ApproveLogin_Implementation(const FUniqueNetIdRepl& InUniqueId, const FString& InOptions) override;
	bool InitNewPlayer(ALobbyistPlayer* NewPlayer, const FString& InOptions) override;
	void Logout(ALobbyistPlayer* ExitingPlayer) override;
	void TravelToGame() override;
};
