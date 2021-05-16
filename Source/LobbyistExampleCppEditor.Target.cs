// Copyright Francesco Camarlinghi. All rights reserved.

using UnrealBuildTool;

public class LobbyistExampleCppEditorTarget : TargetRules
{
	public LobbyistExampleCppEditorTarget(TargetInfo Target)
		: base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange(new[] {"LobbyistExampleCpp"});
	}
}
