// Created by Brian 'Snake' Alexander, (c) 2016

using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
    public class SiegeGameMode : ModuleRules
    {
        public SiegeGameMode(TargetInfo Target)
        {
            PrivateIncludePaths.Add("SiegeGameMode/Private");
            PrivateIncludePathModuleNames.Add("UTDomGameMode");
            PublicIncludePathModuleNames.Add("UTDomGameMode");
            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "Navmesh",
                    "UnrealTournament",
                    "InputCore",
                    "Slate",
                    "SlateCore",
                    "UTDomGameMode",
                }
                );
        }
    }
}