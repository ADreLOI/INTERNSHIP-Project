// Fill out your copyright notice in the Description page of Project Settings.


#include "WeArtSettings.h"

#include "WeArtUtilities.h"

UWeArtSettings::UWeArtSettings(const FObjectInitializer& obj)
{
	// ClientPort = 13031;
	// DebugMessages = false;
	// ControllerAutoStart = true;
	// CalibrationAutoStart = false;
	// RawDataAutoStart = false;
	// EnabledTeleportation = true;
}

void UWeArtSettings::SaveToDefaultConfig() const

{
	// Path to the DefaultWEARTPlugin.ini in the project's Config folder
	FString ConfigFilePath = FPaths::Combine(FPaths::ProjectConfigDir(), TEXT("DefaultWEARTPlugin.ini"));
    
	// Prepare the string that will contain the new config section
	FString ConfigContent;

	// Use FConfigCacheIni to retrieve and format the current settings
	GConfig->Flush(false, GGameIni);  // Flush any pending changes to the GGameIni (or the relevant config)

	// Write each property manually
	ConfigContent += "[/Script/WeArtPlugin.WeArtSettings]\n"; // Section header for your config
    
	ConfigContent += FString::Printf(TEXT("ClientPort=%d\n"), ClientPort);
	ConfigContent += FString::Printf(TEXT("DebugMessages=%s\n"), DebugMessages ? TEXT("True") : TEXT("False"));
	ConfigContent += FString::Printf(TEXT("ControllerAutoStart=%s\n"), ControllerAutoStart ? TEXT("True") : TEXT("False"));
	ConfigContent += FString::Printf(TEXT("CalibrationAutoStart=%s\n"), CalibrationAutoStart ? TEXT("True") : TEXT("False"));
	ConfigContent += FString::Printf(TEXT("RawDataAutoStart=%s\n"), RawDataAutoStart ? TEXT("True") : TEXT("False"));
	ConfigContent += FString::Printf(TEXT("GesturesEnabled=%s\n"), GesturesEnabled ? TEXT("True") : TEXT("False"));
	ConfigContent += FString::Printf(TEXT("DeviceGeneration=%s\n"), *WeArtUtilities::EnumToFString(DeviceGeneration));

	// Now write to the file
	FFileHelper::SaveStringToFile(ConfigContent, *ConfigFilePath);
}
