// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/ExMainMenuTabInterface.h"
#include "ExLobby.generated.h"

class AExLobbyPlayer;
class ALobbyistState;
class ALobbyistPlayer;
class UButton;
class UExLobbyChat;
class UExLobbyPlayerSummary;
class UListView;
class UTextBlock;
class UWidgetSwitcher;

UCLASS()
class UExLobby : public UUserWidget, public IExMainMenuTabInterface
{
	GENERATED_BODY()

public:

	UExLobby(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UButton* LeaveButton = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UButton* StartGameButton = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UExLobbyPlayerSummary* LocalPlayerSummary = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UExLobbyChat* Chat = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UWidgetSwitcher* PlayerListSwitcher = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UListView* PlayerList = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UTextBlock* PlayerCountTextBlock = nullptr;

private:

	/** Cached reference to the lobby state. */
	TWeakObjectPtr<ALobbyistState> WeakLobbyState;

	/** Cached reference to the local lobby player. */
	TWeakObjectPtr<AExLobbyPlayer> WeakLocalPlayer;

	// Handles
	FTimerHandle TimerHandle_UpdatePlayers;

private:

	/** Schedules a players update for next tick. */
	void ScheduleUpdatePlayers();

	/** Updates players widgets with fresh data from the lobby. */
	void UpdatePlayers();

	/** Called when a player joins. */
	UFUNCTION()
	void OnPlayerAdded(ALobbyistPlayer* Player);

	/** Called when a player leaves. */
	UFUNCTION()
	void OnPlayerRemoved(ALobbyistPlayer* Player);

	/** Called when the leave button is clicked. */
	UFUNCTION()
	void OnLeaveButtonClicked();

	/** Called when the start game button is clicked. */
	UFUNCTION()
	void OnStartGameButtonClicked();

	// IExMainMenuTabInterface interface
public:
	void Activated_Implementation() override;
	void Deactivated_Implementation() override;

	// UUserWidget interface
public:
	void NativeOnInitialized() override;
};
