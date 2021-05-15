// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "ExLobbyBrowserEntry.generated.h"

class UButton;
class UTextBlock;

UCLASS(BlueprintType, Transient)
class UExLobbyBrowserEntryData : public UObject
{
	GENERATED_BODY()

public:

	/** Session search result. */
	FOnlineSessionSearchResult SearchResult;

	/** Index of this entry in the server browser results list. */
	UPROPERTY(BlueprintReadOnly, Category = "Player Entry")
	int32 Index = INDEX_NONE;

	/** Event fired when the player wants to join this session. */
	DECLARE_DELEGATE_OneParam(FOnJoinSession, UExLobbyBrowserEntryData* /* SessionEntry */);
	FOnJoinSession OnJoinSession;
};

/////////////////////////////////////////////////////////////////////

UCLASS(Abstract)
class UExLobbyBrowserEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	UExLobbyBrowserEntry(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UTextBlock* HostNameTextBlock = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UTextBlock* PlayerCountTextBlock = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UButton* JoinButton = nullptr;

private:

	/** Server browser entry this widget represents. */
	TWeakObjectPtr<UExLobbyBrowserEntryData> WeakEntryData;

public:

	/** Gets the server browser entry this widget represents. */
	UFUNCTION(BlueprintPure, Category = "User Interface")
	UExLobbyBrowserEntryData* GetEntryData() const;

private:

	/** Called when the join button is clicked. */
	UFUNCTION()
	void OnJoinButtonClicked();

	// IUserObjectListEntry interface
public:
	void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	// UUserWidget interface
public:
	void NativeOnInitialized() override;
};

// UExLobbyBrowserEntry inlines
inline UExLobbyBrowserEntryData* UExLobbyBrowserEntry::GetEntryData() const { return WeakEntryData.Get(); }
