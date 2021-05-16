// Copyright Francesco Camarlinghi. All rights reserved.

#include "Menu/ExLobbyClient.h"

AExLobbyClient::AExLobbyClient(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void AExLobbyClient::ClientReceiveChatMessage_Implementation(const FString& Message)
{
	ChatMessageReceivedEvent.Broadcast(FText::AsCultureInvariant(Message), false);
}

void AExLobbyClient::ClientReceiveJoinMessage_Implementation(const FString& NewPlayerName)
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("PlayerName"), FText::AsCultureInvariant(NewPlayerName));

	ChatMessageReceivedEvent.Broadcast(
		FText::Format(NSLOCTEXT("Lobby", "PlayerJoinedFormat", "{PlayerName} joined!"), Args),
		true
	);
}

void AExLobbyClient::ClientReceiveLeaveMessage_Implementation(const FString& LeavingPlayerName)
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("PlayerName"), FText::AsCultureInvariant(LeavingPlayerName));

	ChatMessageReceivedEvent.Broadcast(
		FText::Format(NSLOCTEXT("Lobby", "PlayerLeftFormat", "{PlayerName} left."), Args),
		true
	);
}
