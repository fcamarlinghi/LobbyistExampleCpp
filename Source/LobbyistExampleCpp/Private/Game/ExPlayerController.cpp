// Copyright Francesco Camarlinghi. All rights reserved.

#include "Game/ExPlayerController.h"
#include "UI/ExInGameMenu.h"

#include "Blueprint/UserWidget.h"

AExPlayerController::AExPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void AExPlayerController::BeginPlay()
{
	if (IsLocalController())
	{
		InGameMenuWidget = CreateWidget<UExInGameMenu>(this, InGameMenuWidgetClass);
		SetInputMode(FInputModeGameOnly());
	}
	Super::BeginPlay();
}

void AExPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent != nullptr)
	{
		InputComponent->BindAction(TEXT("ToggleMenu"), EInputEvent::IE_Pressed,
			this, &ThisClass::ToggleInGameMenu);
	}
}

void AExPlayerController::ToggleInGameMenu()
{
	if (InGameMenuWidget == nullptr)
	{
		return;
	}

	if (InGameMenuWidget->IsInViewport())
	{
		InGameMenuWidget->RemoveFromParent();
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
	else
	{
		InGameMenuWidget->AddToViewport();
		SetInputMode(FInputModeGameAndUI());
		bShowMouseCursor = true;
	}
}
