// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * Firebase Plugin Module
 */
class FFirebasePluginModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Register plugin settings */
	bool RegisterSettings();
	
	/** Unregister plugin settings */
	void UnregisterSettings();
};
