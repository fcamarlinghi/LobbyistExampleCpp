// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExLobbyChat.generated.h"

class AExLobbyPlayer;
class UButton;
class UEditableTextBox;
class UListView;

UCLASS()
class UExLobbyChat : public UUserWidget
{
	GENERATED_BODY()

public:
	UExLobbyChat(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UListView* MessageList = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UEditableTextBox* MessageTextBox = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UButton* SendButton = nullptr;

private:

	/** Cached reference to the lobby player. */
	TWeakObjectPtr<AExLobbyPlayer> WeakPlayer;

	// Handles
	FDelegateHandle DelegateHandle_OnChatMessageReceived;

public:

	/** Initializes the widget for the specified player. */
	void InitializeForPlayer(AExLobbyPlayer* NewPlayer);

	/** Clears the chat. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "User Interface")
	void ClearChat();

private:

	/** Called when a chat message is received. */
	void OnChatMessageReceived(const FText& Message, bool bSystemMessage);

	/** Called when committing text in the message text box. */
	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	/** Called to determine whether the send button is enabled. */
	UFUNCTION()
	bool IsSendButtonEnabled();

	/** Called when the send button is clicked. */
	UFUNCTION()
	void OnSendButtonClicked();

	// UUserWidget interface
public:
	void NativeOnInitialized() override;
};
