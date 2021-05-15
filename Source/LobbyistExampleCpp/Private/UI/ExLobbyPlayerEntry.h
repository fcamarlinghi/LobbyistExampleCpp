// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "ExLobbyPlayerEntry.generated.h"

class AExLobbyPlayer;
class UButton;
class UImage;
class UPanelWidget;
class UTextBlock;

UCLASS(BlueprintType, Transient)
class UExLobbyPlayerEntryData : public UObject
{
	GENERATED_BODY()

public:

	/** Index of this entry in the player list. */
	UPROPERTY(BlueprintReadOnly, Category = "Player Entry")
	int32 Index = INDEX_NONE;

	/** . */
	UPROPERTY(BlueprintReadOnly, Category = "Player Entry")
	AExLobbyPlayer* Player = nullptr;

};

/////////////////////////////////////////////////////////////////////

UCLASS(Abstract)
class UExLobbyPlayerEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	UExLobbyPlayerEntry(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UImage* SkinImage = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UTextBlock* PlayerNameTextBlock = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UTextBlock* PingTextBlock = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UPanelWidget* LeaderBox = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UButton* KickButton = nullptr;

private:

	/** Player entry this widget represents. */
	TWeakObjectPtr<UExLobbyPlayerEntryData> WeakEntryData;

	/** Cached reference to session owner. */
	TWeakObjectPtr<AExLobbyPlayer> WeakLobbyOwner;

public:

	/** Gets the player entry this widget represents. */
	UFUNCTION(BlueprintPure, Category = "User Interface")
	UExLobbyPlayerEntryData* GetEntryData() const;

protected:

	/** Gets the ping text. */
	UFUNCTION()
	FText GetPingText();

	/** Gets the selected skin color. */
	UFUNCTION()
	FLinearColor GetSkinColor();

private:

	/** Called when the kick button is clicked. */
	UFUNCTION()
	void OnKickButtonClicked();

	// IUserObjectListEntry interface
public:
	void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	// UUserWidget interface
public:
	void NativeOnInitialized() override;
};

// UExLobbyPlayerEntry inlines
inline UExLobbyPlayerEntryData* UExLobbyPlayerEntry::GetEntryData() const { return WeakEntryData.Get(); }
