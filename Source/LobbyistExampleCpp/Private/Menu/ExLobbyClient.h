// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "LobbyistClient.h"
#include "ExLobbyClient.generated.h"

UCLASS()
class AExLobbyClient : public ALobbyistClient
{
	GENERATED_BODY()

public:

	AExLobbyClient(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

	/** Broadcasts whenever a new chat message is received. */
	DECLARE_EVENT_TwoParams(AExLobbyClient, FOnChatMessageReceived, const FText& /* Message */, bool /* bSystemMessage */)
	FOnChatMessageReceived& OnChatMessageReceived() { return ChatMessageReceivedEvent; }

private:

	/** Broadcasts whenever a new chat message is received. */
	FOnChatMessageReceived ChatMessageReceivedEvent;

public:

	/** Replicates a chat message. */
	UFUNCTION(Client, Reliable)
	void ClientReceiveChatMessage(const FString& Message);
	virtual void ClientReceiveChatMessage_Implementation(const FString& Message);

	/** Replicates a player join message. */
	UFUNCTION(Client, Reliable)
	void ClientReceiveJoinMessage(const FString& NewPlayerName);
	virtual void ClientReceiveJoinMessage_Implementation(const FString& NewPlayerName);

	/** Replicates a player leave message. */
	UFUNCTION(Client, Reliable)
	void ClientReceiveLeaveMessage(const FString& LeavingPlayerName);
	virtual void ClientReceiveLeaveMessage_Implementation(const FString& LeavingPlayerName);
};
