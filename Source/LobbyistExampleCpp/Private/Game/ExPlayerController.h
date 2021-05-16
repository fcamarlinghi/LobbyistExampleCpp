// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ExPlayerController.generated.h"

class UExInGameMenu;

UCLASS(Abstract)
class AExPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AExPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** In-game menu widget class. */
	UPROPERTY(EditDefaultsOnly, Category = "User Interface")
	TSubclassOf<UExInGameMenu> InGameMenuWidgetClass;

protected:

	/** In-game menu widget instance. */
	UPROPERTY(Transient)
	UExInGameMenu* InGameMenuWidget = nullptr;

protected:

	/** Shows/hides the in-game menu. */
	void ToggleInGameMenu();

	// APlayerController interface
protected:
	void BeginPlay() override;
public:
	void SetupInputComponent() override;
};
