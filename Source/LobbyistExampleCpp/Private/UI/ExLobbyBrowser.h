// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/ExMainMenuTabInterface.h"
#include "ExLobbyBrowser.generated.h"

class FOnlineSessionSearch;
class UButton;
class UExLobbyBrowserEntryData;
class UListView;
class UTextBlock;

UCLASS(Abstract)
class UExLobbyBrowser : public UUserWidget, public IExMainMenuTabInterface
{
	GENERATED_BODY()

public:

	UExLobbyBrowser(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UButton* CreateLobbyButton = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UButton* RefreshButton = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UButton* QuitButton = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UListView* LobbyList = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UTextBlock* NoResultsTextBlock = nullptr;

private:

	/** Active session search. */
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	// Handles
	FDelegateHandle DelegateHandle_OnFindSessionsComplete;

private:

	/** Called when the create lobby button is clicked. */
	UFUNCTION()
	void OnCreateLobbyButtonClicked();

	/** Called when the refresh button is clicked. */
	UFUNCTION()
	void OnRefreshButtonClicked();

	/** Called when the quit button is clicked. */
	UFUNCTION()
	void OnQuitButtonClicked();

	/** Called when a session search is complete. */
	void OnFindSessionsComplete(bool bWasSuccessful);

	/** Cancels any active search for sessions. */
	UFUNCTION()
	void CancelSessionSearch();

	/** Called when the player wants to join the specified session. */
	void OnJoinSession(UExLobbyBrowserEntryData* EntryData);

	// UUserWidget interface
public:
	void NativeOnInitialized() override;
};
