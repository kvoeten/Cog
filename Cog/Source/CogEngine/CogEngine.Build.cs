using UnrealBuildTool;

public class CogEngine : ModuleRules
{
	public CogEngine(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] 
			{
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] 
			{
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
            }
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CogCommon",
                "CogDebug",
                "CogImgui",
                "CogWindow",
				"Core",
                "CoreUObject",
				"Engine",
                "InputCore",
                "NetCore",
				"Slate",
				"SlateCore",
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
