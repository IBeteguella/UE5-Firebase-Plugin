// Copyright. All Rights Reserved.

#include "FirebasePluginModule.h"
#include "FirebaseSettings.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#endif

#define LOCTEXT_NAMESPACE "FFirebasePluginModule"

void FFirebasePluginModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("FirebasePlugin: Module starting up"));
	
	// Register settings
	RegisterSettings();
}

void FFirebasePluginModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("FirebasePlugin: Module shutting down"));
	
	// Unregister settings
	UnregisterSettings();
}

void FFirebasePluginModule::RegisterSettings()
{
#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		// Register Firebase settings
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "Firebase",
			LOCTEXT("FirebaseSettingsName", "Firebase"),
			LOCTEXT("FirebaseSettingsDescription", "Configure Firebase Realtime Database and Authentication settings"),
			GetMutableDefault<UFirebaseSettings>()
		);

		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FFirebasePluginModule::RegisterSettings);
		}
	}
#endif
}

void FFirebasePluginModule::UnregisterSettings()
{
#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Firebase");
	}
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFirebasePluginModule, FirebasePlugin)
