// Copyright Francesco Camarlinghi. All rights reserved.

#include "UI/ExDialog.h"
#include "Menu/ExMenuPlayerController.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

UExWaitingDialog::UExWaitingDialog(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UExWaitingDialog::NativeOnInitialized()
{
	if (CancelButton != nullptr)
	{
		CancelButton->OnClicked.AddDynamic(this, &ThisClass::OnCancelButtonClicked);
	}
	Super::NativeOnInitialized();
}

void UExWaitingDialog::NativeConstruct()
{
	if (MessageTextBlock != nullptr)
	{
		MessageTextBlock->SetText(Message);
	}
	if (CancelButton != nullptr)
	{
		CancelButton->SetIsEnabled(bAllowCancel);
	}
	Super::NativeConstruct();
}

void UExWaitingDialog::OnCancelButtonClicked()
{
	if (AExMenuPlayerController* PlayerController = GetOwningPlayer<AExMenuPlayerController>())
	{
		PlayerController->CloseDialog();
	}
	OnCancel.Broadcast();
}

/////////////////////////////////////////////////////////////////////

UExNotificationDialog::UExNotificationDialog(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UExNotificationDialog::NativeOnInitialized()
{
	if (OkayButton != nullptr)
	{
		OkayButton->OnClicked.AddDynamic(this, &ThisClass::OnOkayButtonClicked);
	}
	Super::NativeOnInitialized();
}

void UExNotificationDialog::NativeConstruct()
{
	if (MessageTextBlock != nullptr)
	{
		MessageTextBlock->SetText(Message);
	}
	Super::NativeConstruct();
}

void UExNotificationDialog::OnOkayButtonClicked()
{
	if (AExMenuPlayerController* PlayerController = GetOwningPlayer<AExMenuPlayerController>())
	{
		PlayerController->CloseDialog();
	}
	OnOkay.Broadcast();
}
