#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeArtCommon.h"

#include "WeArtDeviceTrackingObject.generated.h"

/**
 * @enum TrackingUpdateMethod
 * @brief Enumerates the tracking update methods.
 */
UENUM()
enum TrackingUpdateMethod
{
	PrePhysicsUpdate,
	PostPhysicsUpdate
};

/**
 * @enum OffsetPreset
 * @brief Enumerates the offset presets for different VR devices.
 */
UENUM()
enum OffsetPreset
{
	TD_MetaQuest_SteamVR_Left,				/**< Offset preset for Touch Diver left hand of Meta Quest using SteamVR */
	TD_MetaQuest_SteamVR_Right,				/**< Offset preset for Touch Diver right hand of Meta Quest using SteamVR */
	TD_MetaQuest_OpenXR_Left,				/**< Offset preset for Touch Diver left hand of Meta Quest using OpenXR */
	TD_MetaQuest_OpenXR_Right,				/**< Offset preset for Touch Diver right hand of Meta Quest using OpenXR */
	TDPro_MetaQuest_SteamVR_Left,			/**< Offset preset for Touch Diver Pro left hand of Meta Quest using SteamVR */
	TDPro_MetaQuest_SteamVR_Right,			/**< Offset preset for Touch Diver Pro right hand of Meta Quest using SteamVR */
	TDPro_MetaQuest_OpenXR_Left,			/**< Offset preset for Touch Diver Pro left hand of Meta Quest using OpenXR */
	TDPro_MetaQuest_OpenXR_Right			/**< Offset preset for Touch Diver Pro right hand of Meta Quest using OpenXR */
};

/**
 * @class UWeArtDeviceTrackingObject
 * @brief Component for tracking device objects in the scene.
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class WEARTPLUGIN_API	UWeArtDeviceTrackingObject : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWeArtDeviceTrackingObject();
	~UWeArtDeviceTrackingObject();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * @brief Get the tracking source transformation.
	 * @return The transformation of the tracking source.
	 */
	UFUNCTION(BlueprintCallable)
	FTransform GetTrackingSource();

	UFUNCTION(BlueprintCallable)
	FVector GetLocationOffset() const;

	UFUNCTION(BlueprintCallable)
	FRotator GetRotationOffset() const;

	UFUNCTION(BlueprintCallable)
	void SetOffsetValues(const FVector& OffsetPosition, const FRotator& OffsetRotation);
	
protected:

	// State variables
	UPROPERTY(EditAnywhere, Category = WeArtDeviceTrackingObject, Meta = (DisplayName = "Update method"))
		TEnumAsByte<TrackingUpdateMethod> updateMethod;
	UPROPERTY(BlueprintReadWrite, Category = WeArtDeviceTrackingObject, Meta = (DisplayName = "Tracking source"))
		FTransform trackingSource;

	// Enable external tracker 
	UPROPERTY(EditAnywhere, Category = WeArtDeviceTrackingObject, Meta = (DisplayName = "Disable Tracking"))
		bool disableTracking;
	
	// Offset
	UPROPERTY(EditAnywhere, Category = WeArtDeviceTrackingObject)
		TEnumAsByte<OffsetPreset> OffsetPreset;
	UPROPERTY(EditAnywhere, Category = WeArtDeviceTrackingObject)
		FVector LocationOffset;
	UPROPERTY(EditAnywhere, Category = WeArtDeviceTrackingObject)
		FRotator RotationOffset;

private:
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;
#endif
};
