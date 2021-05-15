// Copyright Francesco Camarlinghi. All rights reserved.

#include "Menu/ExLobbyClient.h"

AExLobbyClient::AExLobbyClient(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void AExLobbyClient::ClientReceiveChatMessage_Implementation(const FString& Message)
{
	OnMessageReceived(FText::AsCultureInvariant(Message), false);
}

void AExLobbyClient::ClientReceiveJoinMessage_Implementation(const FString& NewPlayerName)
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("PlayerName"), FText::AsCultureInvariant(NewPlayerName));

	OnMessageReceived(
		FText::Format(NSLOCTEXT("Lobby", "PlayerJoinedFormat", "{PlayerName} joined!"), Args),
		true
	);
}

void AExLobbyClient::ClientReceiveLeaveMessage_Implementation(const FString& LeavingPlayerName)
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("PlayerName"), FText::AsCultureInvariant(LeavingPlayerName));

	OnMessageReceived(
		FText::Format(NSLOCTEXT("Lobby", "PlayerLeftFormat", "{PlayerName} left."), Args),
		true
	);
}

void AExLobbyClient::OnMessageReceived(const FText& Message, bool bSystemMessage)
{
	// TODO
}
