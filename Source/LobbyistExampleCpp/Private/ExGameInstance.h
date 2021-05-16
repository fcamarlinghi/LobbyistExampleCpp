// Copyright Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ExGameInstance.generated.h"

class UExCharacterSkin;

UCLASS(Abstract)
class UExGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UExGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Available player skins. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data", Meta = (DisplayThumbnail = false))
	TArray<TSoftObjectPtr<UExCharacterSkin>> AvailableSkins;
};
