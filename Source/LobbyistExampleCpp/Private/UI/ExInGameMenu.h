// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExInGameMenu.generated.h"

class UButton;

UCLASS(Abstract)
class UExInGameMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UExInGameMenu(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UButton* ReturnToLobbyButton = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UButton* QuitButton = nullptr;

private:

	/** Called when the return to lobby button is clicked. */
	UFUNCTION()
	void OnReturnToLobbyButtonClicked();

	/** Called when the quit button is clicked. */
	UFUNCTION()
	void OnQuitButtonClicked();

	// UUserWidget interface
public:
	void NativeOnInitialized() override;
	void NativeConstruct() override;
};
