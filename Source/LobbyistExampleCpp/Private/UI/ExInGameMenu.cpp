// Copyright Francesco Camarlinghi. All rights reserved.

#include "UI/ExInGameMenu.h"
#include "Game/ExGameMode.h"

#include "Components/Button.h"
#include "GameFramework/PlayerController.h"

UExInGameMenu::UExInGameMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UExInGameMenu::NativeOnInitialized()
{
	if (ReturnToLobbyButton != nullptr)
	{
		ReturnToLobbyButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnToLobbyButtonClicked);
	}
	if (QuitButton != nullptr)
	{
		QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnQuitButtonClicked);
	}
	Super::NativeOnInitialized();
}

void UExInGameMenu::NativeConstruct()
{
	if (ReturnToLobbyButton != nullptr)
	{
		// Only the host can end the game and return to the lobby
		const UWorld* World = GetWorld();
		if (World == nullptr || World->GetAuthGameMode() == nullptr)
		{
			ReturnToLobbyButton->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	Super::NativeConstruct();
}

void UExInGameMenu::OnReturnToLobbyButtonClicked()
{
	if (const UWorld* World = GetWorld())
	{
		if (AExGameMode* GameMode = World->GetAuthGameMode<AExGameMode>())
		{
			GameMode->ReturnToLobby();
		}
	}
}

void UExInGameMenu::OnQuitButtonClicked()
{
	const UWorld* World = GetWorld();
	if (World != nullptr && World->GetAuthGameMode() != nullptr)
	{
		World->GetAuthGameMode()->ReturnToMainMenuHost();
	}
	else if (APlayerController* PlayerController = GetOwningPlayer())
	{
		// Remote client, simply load the menu map
		PlayerController->ClientTravel(TEXT("/Game/Maps/Menu?closed"), ETravelType::TRAVEL_Absolute);
	}
}
