// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ExCharacter.generated.h"

class UExCharacterSkin;

UCLASS(Abstract)
class AExCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AExCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	/** Skin applied to the character. */
	UPROPERTY(Transient, Replicated, ReplicatedUsing = OnSkinChanged)
	UExCharacterSkin* CharacterSkin = nullptr;

public:

	/** Sets the skin applied to the character. */
	void SetSkin(UExCharacterSkin* NewSkin);

protected:

	/** Called when the skin applied to the character changes. */
	UFUNCTION()
	void OnSkinChanged();

public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
