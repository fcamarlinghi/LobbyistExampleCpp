// Copyright Francesco Camarlinghi. All rights reserved.

#include "ExLocalPlayer.h"

UExLocalPlayer::UExLocalPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

FString UExLocalPlayer::GetGameLoginOptions() const
{
	return FString::JoinBy(GameLoginOptions, TEXT("?"), [](const TPair<FString, FString>& Option)
	{
		return FString::Printf(TEXT("%s=%s"), *Option.Get<0>(), *Option.Get<1>());
	});
}
