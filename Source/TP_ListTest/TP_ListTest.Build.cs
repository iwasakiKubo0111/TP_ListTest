// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TP_ListTest : ModuleRules
{
	public TP_ListTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Slate", "SlateCore" });
	}
}
