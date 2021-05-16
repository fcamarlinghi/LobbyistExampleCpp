// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "ExLobbyChatEntry.generated.h"

class UButton;
class UTextBlock;

UCLASS(BlueprintType, Transient)
class UExLobbyChatEntryData : public UObject
{
	GENERATED_BODY()

public:

	/** Chat message. */
	UPROPERTY(BlueprintReadOnly, Category = "Chat Entry")
	FText Message;

	/** Whether this is a system-generated message or a user message. */
	UPROPERTY(BlueprintReadOnly, Category = "Chat Entry")
	bool bSystemMessage = false;

};

/////////////////////////////////////////////////////////////////////

UCLASS(Abstract)
class UExLobbyChatEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	UExLobbyChatEntry(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UTextBlock* MessageTextBlock = nullptr;

private:

	/** Chat entry this widget represents. */
	TWeakObjectPtr<UExLobbyChatEntryData> WeakEntryData;

public:

	/** Gets the chat entry this widget represents. */
	UFUNCTION(BlueprintPure, Category = "User Interface")
	UExLobbyChatEntryData* GetEntryData() const;

	// IUserObjectListEntry interface
public:
	void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};

// UExLobbyChatEntry inlines
inline UExLobbyChatEntryData* UExLobbyChatEntry::GetEntryData() const { return WeakEntryData.Get(); }
