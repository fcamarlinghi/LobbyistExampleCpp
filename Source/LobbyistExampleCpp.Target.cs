// Copyright Francesco Camarlinghi. All rights reserved.

using UnrealBuildTool;

public class LobbyistExampleCppTarget : TargetRules
{
	public LobbyistExampleCppTarget(TargetInfo Target)
		: base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange(new[] {"LobbyistExampleCpp"});
	}
}
