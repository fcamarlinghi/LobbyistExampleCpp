// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExMainMenu.generated.h"

class UWidgetSwitcher;

UENUM(BlueprintType)
enum class EExMainMenuTab : uint8
{
	ServerBrowser,
	Lobby,
};

UCLASS(Abstract)
class UExMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UExMainMenu(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UWidgetSwitcher* LayoutSwitcher = nullptr;

public:

	/** Changes the active tab. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void SetActiveTab(EExMainMenuTab NewTab);
};
