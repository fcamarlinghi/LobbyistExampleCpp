// Copyright Francesco Camarlinghi. All rights reserved.

#include "Menu/ExMenuPlayerController.h"
#include "UI/ExDialog.h"
#include "UI/ExMainMenu.h"
#include "LobbyistClient.h"
#include "LobbyistSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "OnlineSubsystemUtils.h"

AExMenuPlayerController::AExMenuPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShowMouseCursor = true;
}

void AExMenuPlayerController::BeginPlay()
{
	MenuWidget = CreateWidget<UExMainMenu>(this, MenuWidgetClass);
	if (MenuWidget != nullptr)
	{
		MenuWidget->AddToViewport();
		SetInputMode(FInputModeUIOnly().SetWidgetToFocus(MenuWidget->GetCachedWidget()));
	}

	const UWorld* World = GetWorld();
	if (World != nullptr && World->URL.HasOption(TEXT("closed")))
	{
		// We got back to the main menu after being disconnected
		ResetSession();
	}
	else if (const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld()))
	{
		// If a session is active, it means we have returned to the main menu from a game
		// Recreate the lobby accordingly
		const FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(NAME_GameSession);
		if (NamedSession != nullptr && NamedSession->SessionState != EOnlineSessionState::Destroying)
		{
			if (GetGameInstance()->FindLocalPlayerFromUniqueNetId(NamedSession->OwningUserId))
			{
				HostLobby();
			}
			else
			{
				if (UExWaitingDialog* Dialog = ShowWaitingDialog(NSLOCTEXT("Lobby", "ReconnectingToLobby", "Reconnecting to lobby...")))
				{
					Dialog->OnCancel.AddDynamic(this, &ThisClass::ResetSession);
				}

				JoinLobby();
			}
		}
	}

	Super::BeginPlay();
}

UExWaitingDialog* AExMenuPlayerController::ShowWaitingDialog(const FText& Message, const bool bAllowCancel)
{
	CloseDialog();
	UExWaitingDialog* NewDialog = CreateWidget<UExWaitingDialog>(this, WaitingDialogWidgetClass);
	if (NewDialog != nullptr)
	{
		DialogWidget = NewDialog;
		NewDialog->Message = Message;
		NewDialog->bAllowCancel = bAllowCancel;
		NewDialog->AddToViewport(100);
		SetInputMode(FInputModeUIOnly().SetWidgetToFocus(NewDialog->GetCachedWidget()));
	}
	return NewDialog;
}

UExNotificationDialog* AExMenuPlayerController::ShowNotificationDialog(const FText& Message)
{
	CloseDialog();
	UExNotificationDialog* NewDialog = CreateWidget<UExNotificationDialog>(this, NotificationDialogWidgetClass);
	if (NewDialog != nullptr)
	{
		DialogWidget = NewDialog;
		NewDialog->Message = Message;
		NewDialog->AddToViewport(100);
		SetInputMode(FInputModeUIOnly().SetWidgetToFocus(NewDialog->GetCachedWidget()));
	}
	return NewDialog;
}

void AExMenuPlayerController::CloseDialog()
{
	if (DialogWidget != nullptr)
	{
		DialogWidget->RemoveFromParent();
		DialogWidget = nullptr;

		if (MenuWidget != nullptr)
		{
			SetInputMode(FInputModeUIOnly().SetWidgetToFocus(MenuWidget->GetCachedWidget()));
		}
	}
}

void AExMenuPlayerController::HostSession()
{
	ResetSession();

	// Create a new online session
	const FUniqueNetIdRepl PrimaryPlayerId = GetGameInstance()->GetPrimaryPlayerUniqueIdRepl();
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!PrimaryPlayerId.IsValid() || !SessionInterface.IsValid())
	{
		return;
	}

	if (UExWaitingDialog* Dialog = ShowWaitingDialog(NSLOCTEXT("Lobby", "CreatingSession", "Creating session...")))
	{
		Dialog->OnCancel.AddDynamic(this, &ThisClass::ResetSession);
	}

	FOnlineSessionSettings SessionSettings;
	SessionSettings.NumPublicConnections = 8;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;

	DelegateHandle_OnCreateSessionComplete = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete));
	if (!SessionInterface->CreateSession(*PrimaryPlayerId, NAME_GameSession, SessionSettings))
	{
		ShowNotificationDialog(NSLOCTEXT("Lobby", "FailedToCreateSession", "Failed to create a new online session."));
		ResetSession();
	}
}

void AExMenuPlayerController::OnCreateSessionComplete(const FName SessionName, const bool bWasSuccessful)
{
	if (SessionName != NAME_GameSession)
	{
		// Not the session we're interested in
		return;
	}

	const UWorld* World = GetWorld();
	if (const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(World))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(DelegateHandle_OnCreateSessionComplete);
	}

	if (bWasSuccessful)
	{
		// Now that the session is created, host a lobby
		if (UExWaitingDialog* Dialog = ShowWaitingDialog(NSLOCTEXT("Lobby", "CreatingLobby", "Creating lobby...")))
		{
			Dialog->OnCancel.AddDynamic(this, &ThisClass::ResetSession);
		}

		if (HostLobby())
		{
			return;
		}
	}

	// If we're here, an error happened
	ShowNotificationDialog(NSLOCTEXT("Lobby", "FailedToCreateSession", "Failed to create a new online session."));
	ResetSession();
}

void AExMenuPlayerController::JoinSession(const FOnlineSessionSearchResult& SearchResult)
{
	ResetSession();

	// Join the specified session
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		return;
	}

	if (UExWaitingDialog* Dialog = ShowWaitingDialog(NSLOCTEXT("Lobby", "JoiningSession", "Joining session...")))
	{
		Dialog->OnCancel.AddDynamic(this, &ThisClass::ResetSession);
	}

	DelegateHandle_OnJoinSessionComplete = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete));
	if (!SessionInterface->JoinSession(0, NAME_GameSession, SearchResult))
	{
		ShowNotificationDialog(NSLOCTEXT("Lobby", "FailedToJoinSession", "Failed to join the online session."));
		ResetSession();
	}
}

void AExMenuPlayerController::OnJoinSessionComplete(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionName != NAME_GameSession)
	{
		// Not the session we're interested in
		return;
	}

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		// Now that we've joined the online session, connect to the lobby
		if (UExWaitingDialog* Dialog = ShowWaitingDialog(NSLOCTEXT("Lobby", "ConnectingToLobby", "Connecting to lobby...")))
		{
			Dialog->OnCancel.AddDynamic(this, &ThisClass::ResetSession);
		}

		if (JoinLobby())
		{
			return;
		}
	}

	// If we're here, an error happened
	ShowNotificationDialog(NSLOCTEXT("Lobby", "FailedToJoinSession", "Failed to join the online session."));
	ResetSession();
}

bool AExMenuPlayerController::HostLobby()
{
	if (ULobbyistSubsystem* LobbyistSubsystem = ULobbyistSubsystem::Get(GetWorld()))
	{
		ALobbyistHost* Host = LobbyistSubsystem->CreateLobbyHost();
		ALobbyistClient* Client = LobbyistSubsystem->CreateLobbyClient();
		if (Host != nullptr && Client != nullptr)
		{
			// Immediately join the new lobby
			Client->OnConnectedToLobby.AddDynamic(this, &ThisClass::OnJoinedLobby);
			Client->OnNetworkFailure.AddDynamic(this, &ThisClass::OnLobbyNetworkFailure);
			Client->OnDisconnectedFromLobby.AddDynamic(this, &ThisClass::OnDisconnectedFromLobby);
			Client->ConnectToSession();
			return true;
		}
	}
	return false;
}

bool AExMenuPlayerController::JoinLobby()
{
	if (ULobbyistSubsystem* LobbyistSubsystem = ULobbyistSubsystem::Get(GetWorld()))
	{
		if (ALobbyistClient* Client = LobbyistSubsystem->CreateLobbyClient())
		{
			Client->OnJoinedLobby.AddDynamic(this, &ThisClass::OnJoinedLobby);
			Client->OnNetworkFailure.AddDynamic(this, &ThisClass::OnLobbyNetworkFailure);
			Client->OnDisconnectedFromLobby.AddDynamic(this, &ThisClass::OnDisconnectedFromLobby);
			Client->ConnectToSession();
			return true;
		}
	}
	return false;
}

void AExMenuPlayerController::OnJoinedLobby()
{
	CloseDialog();
	if (MenuWidget != nullptr)
	{
		MenuWidget->SetActiveTab(EExMainMenuTab::Lobby);
	}
}

void AExMenuPlayerController::OnLobbyNetworkFailure(const ENetworkFailure::Type FailureType)
{
	FText ErrorMessage;
	switch (FailureType)
	{
		case ENetworkFailure::ConnectionLost:
		case ENetworkFailure::FailureReceived:
		case ENetworkFailure::OutdatedServer:
			ErrorMessage = NSLOCTEXT("Lobby", "ConnectionLostError", "Connection to the remote host has been lost.");
			break;

		case ENetworkFailure::ConnectionTimeout:
			ErrorMessage = NSLOCTEXT("Lobby", "ConnectionTimeoutError", "Connection to the remote host timed out.");
			break;
		case ENetworkFailure::OutdatedClient:
			ErrorMessage = NSLOCTEXT("Lobby", "OutdatedClientError", "Your game is out of date. Please update to the latest version and try again.");
			break;

		case ENetworkFailure::NetDriverAlreadyExists:
		case ENetworkFailure::NetDriverCreateFailure:
		case ENetworkFailure::NetDriverListenFailure:
		case ENetworkFailure::PendingConnectionFailure:
		case ENetworkFailure::NetGuidMismatch:
		case ENetworkFailure::NetChecksumMismatch:
		default:
			// TODO: different error message if we're hosting?
			ErrorMessage = NSLOCTEXT("Lobby", "GenericNetworkError", "Failed to connect to the remote host.");
			break;
	}
	ShowNotificationDialog(ErrorMessage);
	ResetSession();
}

void AExMenuPlayerController::OnDisconnectedFromLobby(const FText& Reason)
{
	ResetSession();
	ShowNotificationDialog(Reason);
}

void AExMenuPlayerController::ResetSession()
{
	const UWorld* World = GetWorld();
	if (ULobbyistSubsystem* LobbyistSubsystem = ULobbyistSubsystem::Get(World))
	{
		LobbyistSubsystem->LeaveLobby();
	}

	if (const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(World))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(DelegateHandle_OnCreateSessionComplete);
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(DelegateHandle_OnJoinSessionComplete);
		SessionInterface->DestroySession(NAME_GameSession);
	}

	if (MenuWidget != nullptr)
	{
		MenuWidget->SetActiveTab(EExMainMenuTab::ServerBrowser);
	}
}
