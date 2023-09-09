// Copyright Francesco Camarlinghi. All rights reserved.

using UnrealBuildTool;

public class LobbyistExampleCppTarget : TargetRules
{
	public LobbyistExampleCppTarget(TargetInfo Target)
		: base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange(new[] { "LobbyistExampleCpp" });
	}
}
