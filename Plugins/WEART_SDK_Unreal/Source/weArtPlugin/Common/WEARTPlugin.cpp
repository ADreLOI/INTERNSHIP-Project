// Copyright Epic Games, Inc. All Rights Reserved.

#include "weArtPlugin.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "WeArtSettings.h"

#define LOCTEXT_NAMESPACE "FWEARTPluginModule"

DEFINE_LOG_CATEGORY(LogWeArt);

void FWEARTPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	if (ISettingsModule* SettingModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingModule->RegisterSettings("Project", "Plugins", "WeArtSettings",
			LOCTEXT("RuntimeSettingsName", "WeArt"),
			LOCTEXT("RuntimeSettingsDescription", "Configure WeArt Settings"),
			GetMutableDefault<UWeArtSettings>()
		);
	}
}

void FWEARTPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "WeArtSettings");
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FWEARTPluginModule, WEARTPlugin)