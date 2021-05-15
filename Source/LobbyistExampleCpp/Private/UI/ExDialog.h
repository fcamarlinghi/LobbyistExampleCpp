// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExDialog.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExOnDialogPrimaryAction);

UCLASS(Abstract)
class UExWaitingDialog : public UUserWidget
{
	GENERATED_BODY()

public:

	UExWaitingDialog(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Message to display. */
	FText Message;

	/** Whether the dialog can be canceled or not. */
	bool bAllowCancel = false;

	/** Event fired when the cancel button is clicked. */
	UPROPERTY(BlueprintAssignable)
	FExOnDialogPrimaryAction OnCancel;

protected:

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UTextBlock* MessageTextBlock = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UButton* CancelButton = nullptr;

private:

	/** Called when the cancel is clicked. */
	UFUNCTION()
	void OnCancelButtonClicked();

	// UUserWidget interface
public:
	void NativeConstruct() override;
	void NativeOnInitialized() override;
};

/////////////////////////////////////////////////////////////////////

UCLASS(Abstract)
class UExNotificationDialog : public UUserWidget
{
	GENERATED_BODY()

public:

	UExNotificationDialog(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Message to display. */
	FText Message;

	/** Event fired when the okay button is clicked. */
	UPROPERTY(BlueprintAssignable)
	FExOnDialogPrimaryAction OnOkay;

protected:

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UTextBlock* MessageTextBlock = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UButton* OkayButton = nullptr;

private:

	/** Called when the okay button is clicked. */
	UFUNCTION()
	void OnOkayButtonClicked();

	// UUserWidget interface
public:
	void NativeOnInitialized() override;
	void NativeConstruct() override;
};
