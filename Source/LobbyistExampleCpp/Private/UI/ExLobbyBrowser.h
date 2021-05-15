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

	/** . */
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	// Handles
	FDelegateHandle DelegateHandle_OnFindSessionsComplete;

private:

	/** . */
	UFUNCTION()
	void OnCreateLobbyButtonClicked();

	/** . */
	UFUNCTION()
	void OnRefreshButtonClicked();

	/** . */
	UFUNCTION()
	void OnQuitButtonClicked();

	/** . */
	void OnFindSessionsComplete(bool bWasSuccessful);

	/** . */
	void OnJoinSession(UExLobbyBrowserEntryData* EntryData);

	// UUserWidget interface
public:
	void NativeOnInitialized() override;
};
