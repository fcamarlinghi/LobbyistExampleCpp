// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ExMenuPlayerController.generated.h"

class UExMainMenu;
class UExNotificationDialog;
class UExWaitingDialog;
class UUserWidget;

UCLASS(Abstract)
class AExMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AExMenuPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Main menu widget class. */
	UPROPERTY(EditDefaultsOnly, Category = "User Interface")
	TSubclassOf<UExMainMenu> MenuWidgetClass;

	/** Waiting dialog class. */
	UPROPERTY(EditDefaultsOnly, Category = "User Interface")
	TSubclassOf<UExWaitingDialog> WaitingDialogWidgetClass;

	/** Notification dialog class. */
	UPROPERTY(EditDefaultsOnly, Category = "User Interface")
	TSubclassOf<UExNotificationDialog> NotificationDialogWidgetClass;

protected:

	/** Main menu widget instance. */
	UPROPERTY(Transient)
	UExMainMenu* MenuWidget = nullptr;

	/** Active modal dialog (if any). */
	UPROPERTY(Transient)
	UUserWidget* DialogWidget = nullptr;

	// Handles
	FDelegateHandle DelegateHandle_OnCreateSessionComplete;
	FDelegateHandle DelegateHandle_OnJoinSessionComplete;

public:

	/** Shows a modal dialog with a waiting animation. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "User Interface")
	UExWaitingDialog* ShowWaitingDialog(const FText& Message, bool bAllowCancel = true);

	/** Shows a notification modal dialog. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "User Interface")
	UExNotificationDialog* ShowNotificationDialog(const FText& Message);

	/** Closes the active modal dialog (if any). */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "User Interface")
	void CloseDialog();

	/** Hosts an online session. */
	void HostSession();

	/** Joins the specified session. */
	void JoinSession(const FOnlineSessionSearchResult& SearchResult);

	/** Resets the active session (if any). */
	UFUNCTION()
	void ResetSession();

protected:

	/** Called when a create session request has completed. */
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/** Called when a join session request has completed. */
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/** . */
	void HostLobby();

	/** . */
	void JoinLobby();

	/** Called when the connection to the lobby is first established. */
	UFUNCTION()
	void OnConnectedToLobby();

	/** Called after disconnecting from the lobby because of a network error. */
	UFUNCTION()
	void OnLobbyNetworkFailure(ENetworkFailure::Type FailureType);

	/** Called after being gracefully disconnected from the lobby. */
	UFUNCTION()
	void OnDisconnectedFromLobby(const FText& Reason);

	// AActor interface
protected:
	void BeginPlay() override;
};
