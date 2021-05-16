// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ExMainMenuTabInterface.generated.h"

UINTERFACE()
class UExMainMenuTabInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implemented by main menu tabs to receive notifications from the parent widget.
 */
class IExMainMenuTabInterface
{
	GENERATED_BODY()

public:

	/** Called when this tab is activated. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic, Category = "UI")
	void Activated();
	virtual void Activated_Implementation();

	/** Called when this tab is deactivated. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic, Category = "UI")
	void Deactivated();
	virtual void Deactivated_Implementation();
};
