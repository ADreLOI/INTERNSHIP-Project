// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class WEARTPlugin : ModuleRules
{
	public WEARTPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDefinitions.Add("WITH_RUNTIME_PHYSICS_COOKING=1");
		CppStandard = CppStandardVersion.Latest;
		bEnableExceptions = true;
		PublicIncludePaths.AddRange(
			new string[] {
                Path.Combine(ModuleDirectory, "Common"),
                Path.Combine(ModuleDirectory, "Components"),
                Path.Combine(ModuleDirectory, "Features"),
                Path.Combine(ModuleDirectory, "Messages"),
                Path.Combine(ModuleDirectory, "nlohmann")
            }
			);
		
		PrivateIncludePaths.AddRange(
			new string[] {
                Path.Combine(ModuleDirectory, "Common"),
                Path.Combine(ModuleDirectory, "Components"),
                Path.Combine(ModuleDirectory, "Features"),
                Path.Combine(ModuleDirectory, "Messages"),
                Path.Combine(ModuleDirectory, "nlohmann")
            }
			);
		
		PublicDependencyModuleNames.AddRange(
			new string[] {
			"Core",
			"Networking",
			"Sockets",
			"Slate",
			"SlateCore",
			"UMG",
			"Json"
			// ... add other public dependencies that you statically link with here ...
		}
		);

		if (Target.Version.MajorVersion == 5)
		{
			PublicDependencyModuleNames.Add("Chaos");
		}

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "Networking",
                "Sockets"
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
		
	}
	
}
