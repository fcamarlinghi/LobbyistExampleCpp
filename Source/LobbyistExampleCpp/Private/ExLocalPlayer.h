// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "ExLocalPlayer.generated.h"

UCLASS()
class UExLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:

	UExLocalPlayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Options passed during player login. */
	TMap<FString, FString> GameLoginOptions;

	// ULocalPlayer interface
public:
	FString GetGameLoginOptions() const override;
};
