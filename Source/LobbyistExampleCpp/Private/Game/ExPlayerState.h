// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ExPlayerState.generated.h"

class UExCharacterSkin;

UCLASS()
class AExPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	AExPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Skin selected by the player. */
	UPROPERTY(Transient)
	UExCharacterSkin* CharacterSkin = nullptr;
};
