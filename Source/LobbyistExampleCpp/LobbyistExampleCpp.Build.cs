// Copyright Francesco Camarlinghi. All rights reserved.

using UnrealBuildTool;

public class LobbyistExampleCpp : ModuleRules
{
	public LobbyistExampleCpp(ReadOnlyTargetRules Target)
		: base(Target)
	{
		bLegacyPublicIncludePaths = false;
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("LobbyistExampleCpp/Private");

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			"Lobbyist",
			"NetCore",
			"SlateCore",
			"UMG",
		});
	}
}
