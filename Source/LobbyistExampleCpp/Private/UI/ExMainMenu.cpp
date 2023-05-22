// Copyright Francesco Camarlinghi. All rights reserved.

#include "UI/ExMainMenu.h"
#include "UI/ExMainMenuTabInterface.h"

#include "Components/WidgetSwitcher.h"

UExMainMenu::UExMainMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UExMainMenu::SetActiveTab(const EExMainMenuTab NewTab)
{
	if (LayoutSwitcher != nullptr
		&& LayoutSwitcher->GetActiveWidgetIndex() != static_cast<int32>(NewTab))
	{
		{
			UWidget* PreviousTab = LayoutSwitcher->GetActiveWidget();
			if (PreviousTab != nullptr && PreviousTab->Implements<UExMainMenuTabInterface>())
			{
				IExMainMenuTabInterface::Execute_Deactivated(PreviousTab);
			}
		}

		LayoutSwitcher->SetActiveWidgetIndex(static_cast<int32>(NewTab));

		{
			UWidget* ActiveTab = LayoutSwitcher->GetActiveWidget();
			if (ActiveTab != nullptr && ActiveTab->Implements<UExMainMenuTabInterface>())
			{
				IExMainMenuTabInterface::Execute_Activated(ActiveTab);
			}
		}
	}
}
