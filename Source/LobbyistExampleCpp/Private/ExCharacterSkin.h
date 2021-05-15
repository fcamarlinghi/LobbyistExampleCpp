// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ExCharacterSkin.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UExCharacterSkin : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Skin name. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Skin")
	FText Name;

	/** Skin color. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Skin")
	FLinearColor Color = FLinearColor::White;
};
