// Copyright Francesco Camarlinghi. All rights reserved.

#include "UI/ExLobbyBrowser.h"
#include "Menu/ExMenuPlayerController.h"
#include "UI/ExDialog.h"
#include "UI/ExLobbyBrowserEntry.h"

#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "OnlineSubsystemUtils.h"

UExLobbyBrowser::UExLobbyBrowser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UExLobbyBrowser::NativeOnInitialized()
{
	if (CreateLobbyButton != nullptr)
	{
		CreateLobbyButton->OnClicked.AddDynamic(this, &ThisClass::OnCreateLobbyButtonClicked);
	}
	if (RefreshButton != nullptr)
	{
		RefreshButton->OnClicked.AddDynamic(this, &ThisClass::OnRefreshButtonClicked);
	}
	if (QuitButton != nullptr)
	{
		QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnQuitButtonClicked);
	}
	Super::NativeOnInitialized();
}

void UExLobbyBrowser::OnCreateLobbyButtonClicked()
{
	if (AExMenuPlayerController* PlayerController = GetOwningPlayer<AExMenuPlayerController>())
	{
		PlayerController->HostSession();
	}
}

void UExLobbyBrowser::OnRefreshButtonClicked()
{
	AExMenuPlayerController* PlayerController = GetOwningPlayer<AExMenuPlayerController>();
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (PlayerController == nullptr || !SessionInterface.IsValid())
	{
		return;
	}

	if (LobbyList != nullptr)
	{
		LobbyList->ClearListItems();
	}

	CancelSessionSearch();
	SessionSearch = MakeShared<FOnlineSessionSearch>();
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	DelegateHandle_OnFindSessionsComplete = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete));
	if (SessionInterface->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		UExWaitingDialog* Dialog = PlayerController->ShowWaitingDialog(
			NSLOCTEXT("Lobby", "LookingForSessions", "Looking for games..."));
		if (Dialog != nullptr)
		{
			Dialog->OnCancel.AddDynamic(this, &ThisClass::CancelSessionSearch);
		}
	}
	else
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(DelegateHandle_OnFindSessionsComplete);
	}
}

void UExLobbyBrowser::OnQuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UExLobbyBrowser::OnFindSessionsComplete(const bool bWasSuccessful)
{
	if (const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(DelegateHandle_OnFindSessionsComplete);
	}

	if (AExMenuPlayerController* PlayerController = GetOwningPlayer<AExMenuPlayerController>())
	{
		PlayerController->CloseDialog();
	}

	if (!SessionSearch.IsValid())
	{
		// Search was probably canceled
		return;
	}

	if (!bWasSuccessful)
	{
		if (AExMenuPlayerController* PlayerController = GetOwningPlayer<AExMenuPlayerController>())
		{
			PlayerController->ShowNotificationDialog(
				NSLOCTEXT("Lobby", "SessionSearchFailed", "Failed to search for games. Please check your network connection and try again."));
		}
		return;
	}

	if (LobbyList != nullptr)
	{
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
		{
#if !UE_BUILD_SHIPPING
			DumpSession(&SessionSearch->SearchResults[i].Session);
#endif

			UExLobbyBrowserEntryData* EntryData = NewObject<UExLobbyBrowserEntryData>(this);
			EntryData->SearchResult = SessionSearch->SearchResults[i];
			EntryData->Index = i;
			EntryData->OnJoinSession.BindUObject(this, &ThisClass::OnJoinSession);
			LobbyList->AddItem(EntryData);
		}
	}

	if (NoResultsTextBlock != nullptr)
	{
		NoResultsTextBlock->SetVisibility(
			SessionSearch->SearchResults.Num() > 0 ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
}

void UExLobbyBrowser::CancelSessionSearch()
{
	if (SessionSearch.IsValid())
	{
		if (const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld()))
		{
			SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(DelegateHandle_OnFindSessionsComplete);
			SessionInterface->CancelFindSessions();
		}
		SessionSearch.Reset();
	}

	if (NoResultsTextBlock != nullptr)
	{
		NoResultsTextBlock->SetVisibility(ESlateVisibility::Visible);
	}
}

void UExLobbyBrowser::OnJoinSession(UExLobbyBrowserEntryData* EntryData)
{
	AExMenuPlayerController* PlayerController = GetOwningPlayer<AExMenuPlayerController>();
	if (PlayerController != nullptr && EntryData != nullptr)
	{
		PlayerController->JoinSession(EntryData->SearchResult);
	}
}
