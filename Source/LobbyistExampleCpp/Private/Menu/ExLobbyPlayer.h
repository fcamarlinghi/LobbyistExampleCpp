// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "LobbyistPlayer.h"
#include "ExLobbyPlayer.generated.h"

class UExCharacterSkin;

UCLASS()
class AExLobbyPlayer : public ALobbyistPlayer
{
	GENERATED_BODY()

public:

	AExLobbyPlayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	/** Character skin selected by the player. */
	UPROPERTY(Replicated, ReplicatedUsing = OnCharacterSkinChanged)
	UExCharacterSkin* CharacterSkin = nullptr;

	/** Time stamp at which ping was last updated. */
	float LastPingTimeStamp = 0.0f;

	/** Exact ping in ms (available on authority and owner client). */
	float ExactPing = 0.0f;

	/** Replicated ping in ms, divided by 4. */
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Ping)
	uint8 Ping = 0;

	// Handles
	FTimerHandle TimerHandle_RequestPingUpdate;

public:

	/** Gets the player's ping. */
	UFUNCTION(BlueprintPure, Category = "Lobby Player")
	int32 GetPing() const;

	/** Gets the character skin selected by the player. */
	UFUNCTION(BlueprintPure, Category = "Lobby Player")
	UExCharacterSkin* GetCharacterSkin() const;

	/** Gets the character skin selected by the player. */
	UFUNCTION(BlueprintCallable, Category = "Lobby Player")
	void SetCharacterSkin(UExCharacterSkin* NewSkin);

	/** Asks the host to kick the specified player. */
	UFUNCTION(Server, Reliable)
	void ServerRequestKickPlayer(AExLobbyPlayer* PlayerToKick);
	virtual void ServerRequestKickPlayer_Implementation(AExLobbyPlayer* PlayerToKick);

	/** Sends the specified chat message. */
	UFUNCTION(Server, Reliable)
	void ServerSendChatMessage(const FString& Message);
	virtual void ServerSendChatMessage_Implementation(const FString& Message);

	/** Asks the host to start the game. */
	UFUNCTION(Server, Reliable)
	void ServerStartGame();
	virtual void ServerStartGame_Implementation();

protected:

	UFUNCTION(Server, Reliable)
	void ServerSetCharacterSkin(UExCharacterSkin* NewSkin);
	virtual void ServerSetCharacterSkin_Implementation(UExCharacterSkin* NewSkin);

	/** Called when the character skin changes. */
	UFUNCTION()
	void OnCharacterSkinChanged();

	/** Requests a ping update from the server and schedules the next update. */
	void RequestPingUpdate();

	UFUNCTION(Server, Unreliable)
	void ServerPingRequest(float TimeStamp);
	virtual void ServerPingRequest_Implementation(float TimeStamp);

	UFUNCTION(Client, Unreliable)
	void ClientPingResponse(float TimeStamp);
	virtual void ClientPingResponse_Implementation(float TimeStamp);

	/** Updates the ping at the end of client/server round trip. */
	void UpdatePing(float ElapsedTime);

	UFUNCTION(Server, Reliable)
	void ServerUpdatePing(float TimeStamp, float ElapsedTime);
	virtual void ServerUpdatePing_Implementation(float TimeStamp, float ElapsedTime);

	/** Called when the approximated ping is replicated to remote, non-owning clients. */
	UFUNCTION()
	void OnRep_Ping();

	// AActor interface
protected:
	void BeginPlay() override;
public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

// AExLobbyPlayer inlines
inline UExCharacterSkin* AExLobbyPlayer::GetCharacterSkin() const { return CharacterSkin; }
inline int32 AExLobbyPlayer::GetPing() const { return FMath::TruncToInt(ExactPing); }
