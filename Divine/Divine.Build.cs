// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Divine : ModuleRules
{
	public Divine(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                "Divine"
            }
        );

        PublicDependencyModuleNames.AddRange(new string[] 
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "AIModule",
            "GameplayTasks",
            "UMG",
            "GameplayTags",
            "OnlineSubsystem",
            "DeveloperSettings",
            "SignificanceManager",
            "EnhancedInput",
            "Niagara",
            "CoreOnline"
        });

		PrivateDependencyModuleNames.AddRange(new string[] 
        {
            "MoviePlayer", 
			"RenderCore" 
        });

	}
}
