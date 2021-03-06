// Created by Brian 'Snake' Alexander, 2015
using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
    public class UTDomGameMode : ModuleRules
    {
        public UTDomGameMode(TargetInfo Target)
        {
            PrivateIncludePaths.Add("UTDomGameMode/Private");
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
                    "MultiTeamGame"
                }
                );
            PrivateDependencyModuleNames.AddRange(
                new string[]
                { "MultiTeamGame" }
                );
        }
    }
}
