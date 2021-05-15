// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExLobby.generated.h"

class UButton;
class UExLobbyPlayerSummary;
class UListView;
class UTextBlock;
class UWidgetSwitcher;

UCLASS()
class UExLobby : public UUserWidget
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
	UWidgetSwitcher* PlayerListSwitcher = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UListView* PlayerList = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UTextBlock* PlayerCountTextBlock = nullptr;
};
