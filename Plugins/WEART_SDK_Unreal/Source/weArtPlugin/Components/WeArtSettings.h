
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WeArtSettings.generated.h"

UENUM(BlueprintType)
enum class EDeviceGeneration: uint8
{
	TouchDiverPro,
	TouchDiver
};

/// <summary>
/// Container class that handles the settings of the SDK
/// </summary>
UCLASS(config = WEARTPlugin, Blueprintable)
class WEARTPLUGIN_API UWeArtSettings : public UObject
{
	GENERATED_BODY()

public:
	UWeArtSettings(const FObjectInitializer& obj);
	void SaveToDefaultConfig() const;

	/** Type of Touch Diver */
	UPROPERTY(Config, EditAnywhere, Category = "WeArt Settings")
		EDeviceGeneration DeviceGeneration = EDeviceGeneration::TouchDiver;
	
	/** Client port setting */
	UPROPERTY(Config, EditAnywhere, Category = "WeArt Settings")
		int32 ClientPort = 13031;

	/** Debug messages setting */
	UPROPERTY(Config, EditAnywhere, Category = "WeArt Settings")
		bool DebugMessages = true;

	/** Controller auto start setting */
	UPROPERTY(Config, EditAnywhere, Category = "WeArt Settings")
		bool ControllerAutoStart = true;

	/** Calibration auto start setting */
	UPROPERTY(Config, EditAnywhere, Category = "WeArt Settings")
		bool CalibrationAutoStart = false;

	/** Raw data auto start setting */
	UPROPERTY(Config, EditAnywhere, Category = "WeArt Settings")
		bool RawDataAutoStart = false;

	/** Enabled gestures setting */
	UPROPERTY(Config, EditAnywhere, Category = "WeArt Settings")
		bool GesturesEnabled = false;

		
#if UE_EDITOR
	/** Called when a property is edited in the editor */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);

		// Get the name of the property that was changed
		FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

		// Automatically save the settings when a property is changed
		if (PropertyName != NAME_None)
		{
			// Call the SaveSettings function when any property is changed
			SaveToDefaultConfig();
		}
	}
#endif
};
