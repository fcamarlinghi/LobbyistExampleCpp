// Copyright Francesco Camarlinghi. All rights reserved.

#include "Game/ExCharacter.h"
#include "ExCharacterSkin.h"

#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"

AExCharacter::AExCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void AExCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CharacterSkin);
}

void AExCharacter::SetSkin(UExCharacterSkin* NewSkin)
{
	CharacterSkin = NewSkin;
	OnSkinChanged();
}

void AExCharacter::OnSkinChanged()
{
	if (!IsNetMode(ENetMode::NM_DedicatedServer)
		&& CharacterSkin != nullptr
		&& GetMesh() != nullptr)
	{
		if (UMaterialInstanceDynamic* MaterialInstance = GetMesh()->CreateDynamicMaterialInstance(0))
		{
			MaterialInstance->SetVectorParameterValue(TEXT("BodyColor"), CharacterSkin->Color);
		}
	}
}
