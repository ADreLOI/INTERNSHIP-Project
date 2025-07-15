using UnrealBuildTool;

public class UEReceiver : ModuleRules
{
    public UEReceiver(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "Sockets",
                "Networking",
                "Json",
                "JsonUtilities"
            });
    }
}
