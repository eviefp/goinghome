// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class GoingHome : ModuleRules
{
	public GoingHome(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Json", "JsonUtilities", "Slate", "SlateCore", "UMG" });

        PublicIncludePaths.AddRange(new string[] { "QuestSystem" });

        PrivateDependencyModuleNames.AddRange(new string[] { "DonAINavigation" });
        PrivateIncludePathModuleNames.AddRange(new string[] { "DonAINavigation" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
