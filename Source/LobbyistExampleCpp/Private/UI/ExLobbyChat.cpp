// Copyright Francesco Camarlinghi. All rights reserved.

#include "UI/ExLobbyChat.h"
#include "Menu/ExLobbyClient.h"
#include "Menu/ExLobbyPlayer.h"
#include "UI/ExLobbyChatEntry.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ListView.h"

UExLobbyChat::UExLobbyChat(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UExLobbyChat::NativeOnInitialized()
{
	if (MessageTextBox != nullptr)
	{
		MessageTextBox->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted);
	}
	if (SendButton != nullptr)
	{
		SendButton->bIsEnabledDelegate.BindDynamic(this, &ThisClass::IsSendButtonEnabled);
		SendButton->OnClicked.AddDynamic(this, &ThisClass::OnSendButtonClicked);
	}
	Super::NativeOnInitialized();
}

void UExLobbyChat::InitializeForPlayer(AExLobbyPlayer* NewPlayer)
{
	if (AExLobbyPlayer* Player = WeakPlayer.Get())
	{
		if (AExLobbyClient* LobbyClient = Player->GetLobbyClient<AExLobbyClient>())
		{
			LobbyClient->OnChatMessageReceived().Remove(DelegateHandle_OnChatMessageReceived);
		}
		DelegateHandle_OnChatMessageReceived.Reset();
	}

	WeakPlayer = MakeWeakObjectPtr(NewPlayer);

	if (NewPlayer != nullptr)
	{
		if (AExLobbyClient* LobbyClient = NewPlayer->GetLobbyClient<AExLobbyClient>())
		{
			DelegateHandle_OnChatMessageReceived = LobbyClient->OnChatMessageReceived().AddUObject(
				this, &ThisClass::OnChatMessageReceived);
		}
	}
}

void UExLobbyChat::ClearChat()
{
	if (MessageList != nullptr)
	{
		MessageList->ClearListItems();
	}
	if (MessageTextBox != nullptr)
	{
		MessageTextBox->SetText(FText::GetEmpty());
	}
}

void UExLobbyChat::OnChatMessageReceived(const FText& Message, const bool bSystemMessage)
{
	if (MessageList != nullptr)
	{
		UExLobbyChatEntryData* MessageEntry = NewObject<UExLobbyChatEntryData>(this);
		MessageEntry->Message = Message;
		MessageEntry->bSystemMessage = bSystemMessage;
		MessageList->AddItem(MessageEntry);
		MessageList->ScrollToBottom();
	}
}

void UExLobbyChat::OnTextCommitted(const FText& Text, const ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter && !Text.IsEmpty())
	{
		if (AExLobbyPlayer* Player = WeakPlayer.Get())
		{
			Player->ServerSendChatMessage(Text.ToString());

			if (MessageTextBox != nullptr)
			{
				MessageTextBox->SetText(FText::GetEmpty());
			}
		}
	}
}

bool UExLobbyChat::IsSendButtonEnabled()
{
	return MessageTextBox != nullptr && !MessageTextBox->GetText().IsEmpty();
}

void UExLobbyChat::OnSendButtonClicked()
{
	if (MessageTextBox != nullptr && !MessageTextBox->GetText().IsEmpty())
	{
		if (AExLobbyPlayer* Player = WeakPlayer.Get())
		{
			Player->ServerSendChatMessage(MessageTextBox->GetText().ToString());
		}

		MessageTextBox->SetText(FText::GetEmpty());
	}
}
