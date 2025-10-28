using UnrealBuildTool;
using System.IO;

public class FirebasePlugin : ModuleRules
{
	public FirebasePlugin(ReadOnlyTargetRules Target) : base(Target)
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
				"CoreUObject",
				"Engine",
				"Json",
				"JsonUtilities"
			}
		);		
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"Projects"
			}
		);
		
		// Settings module is only needed in editor builds
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("Settings");
		}
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PrivateDependencyModuleNames.Add("Launch");
			
			string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "FirebasePlugin_UPL.xml"));
		}
	}
}
