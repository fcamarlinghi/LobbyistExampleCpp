// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExLobbyPlayerSummary.generated.h"

class AExLobbyPlayer;
class UButton;
class UImage;
class UPanelWidget;
class UTextBlock;

UCLASS()
class UExLobbyPlayerSummary : public UUserWidget
{
	GENERATED_BODY()

public:

	UExLobbyPlayerSummary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UPanelWidget* SummaryBox = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UImage* SkinImage = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UButton* PreviousSkinButton = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UTextBlock* SkinNameTextBlock = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UButton* NextSkinButton = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UTextBlock* PlayerNameTextBlock = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UTextBlock* PingTextBlock = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UPanelWidget* LeaderBox = nullptr;

private:

	/** Cached reference to the lobby player. */
	TWeakObjectPtr<AExLobbyPlayer> WeakPlayer;

public:

	/** Initializes the widget for the specified player. */
	void InitializeForPlayer(AExLobbyPlayer* NewPlayer);

private:

	/** Gets the ping text. */
	UFUNCTION()
	FText GetPingText();

	/** Gets the name of the selected skin. */
	UFUNCTION()
	FText GetSkinName();

	/** Gets the selected skin color. */
	UFUNCTION()
	FLinearColor GetSkinColor();

	/** Called when the previous skin button is clicked. */
	UFUNCTION()
	void OnPreviousSkinButtonClicked();

	/** Called when the next skin button is clicked. */
	UFUNCTION()
	void OnNextSkinButtonClicked();

	// UUserWidget interface
public:
	void NativeOnInitialized() override;
};
