using UnrealBuildTool;

public class CogWindow : ModuleRules
{
	public CogWindow(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CogImgui",
                "CogDebug",
			}
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"InputCore",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "NetCore",
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
