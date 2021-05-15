// Copyright Francesco Camarlinghi. All rights reserved.

#include "UI/ExLobbyBrowser.h"
#include "Menu/ExMenuPlayerController.h"
#include "UI/ExLobbyBrowserEntry.h"

#include "Components/Button.h"
#include "Components/ListView.h"
#include "Kismet/KismetSystemLibrary.h"
#include "OnlineSubsystemUtils.h"
#include "Components/TextBlock.h"

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

	if (SessionSearch.IsValid())
	{
		SessionInterface->CancelFindSessions();
		SessionSearch.Reset();
	}

	SessionSearch = MakeShared<FOnlineSessionSearch>();
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	DelegateHandle_OnFindSessionsComplete = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete));
	if (SessionInterface->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		PlayerController->ShowWaitingDialog(NSLOCTEXT("Lobby", "LookingForGames", "Looking for games..."));
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

	if (!SessionSearch.IsValid())
	{
		// Search was probably canceled
		return;
	}

	if (!bWasSuccessful)
	{
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
			SessionSearch->SearchResults.Num() > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
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
