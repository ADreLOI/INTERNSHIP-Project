#include "WeArtDeviceTrackingObject.h"
#include <map>
#include <utility>

std::map<OffsetPreset, std::pair<FVector, FRotator>> offsetPresetsMap = {
	{TD_MetaQuest_SteamVR_Left, std::make_pair(FVector(-8.0f, -4.0f, -5.0f), FRotator(75.0f, 15.0f, -30.0f))},
	{TD_MetaQuest_SteamVR_Right, std::make_pair(FVector(-10.0, 5.0f, -6.5f), FRotator(-98.0f, 200.0f, 5.0f))},
	{TD_MetaQuest_OpenXR_Left, std::make_pair(FVector(0.0f, -7.0f, -5.0f), FRotator(80.0f, 5.0f, -10.0f))},
	{TD_MetaQuest_OpenXR_Right, std::make_pair(FVector(-2.0, 13.0f, -10.0f), FRotator(-90.0f, 170.0f, 0.0f))},
	{TDPro_MetaQuest_SteamVR_Left, std::make_pair(FVector(-8.0f, -4.0f, -5.0f), FRotator(75.0f, 15.0f, -30.0f))},
	{TDPro_MetaQuest_SteamVR_Right, std::make_pair(FVector(-10.0, 5.0f, -6.5f), FRotator(-98.0f, 200.0f, 5.0f))},
	{TDPro_MetaQuest_OpenXR_Left, std::make_pair(FVector(0.0f, -7.0f, -5.0f), FRotator(80.0f, 5.0f, -10.0f))},
	{TDPro_MetaQuest_OpenXR_Right, std::make_pair(FVector(-2.0, 13.0f, -10.0f), FRotator(-90.0f, 170.0f, 0.0f))}
};

UWeArtDeviceTrackingObject::UWeArtDeviceTrackingObject(): updateMethod{}, disableTracking(false), OffsetPreset{}
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = (updateMethod == TrackingUpdateMethod::PrePhysicsUpdate
		                                  ? TG_PrePhysics
		                                  : TG_PostPhysics);
}

UWeArtDeviceTrackingObject::~UWeArtDeviceTrackingObject()
{
}

void UWeArtDeviceTrackingObject::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FTransform UWeArtDeviceTrackingObject::GetTrackingSource()
{
	auto posOffset = LocationOffset;
	auto rotOffset = RotationOffset;

	FVector newLoc = trackingSource.TransformPosition(posOffset);
	FQuat newRot = trackingSource.TransformRotation(FQuat::MakeFromEuler(FVector(rotOffset.Pitch, rotOffset.Yaw, rotOffset.Roll)));
	FTransform newTrans(newRot, newLoc);
	return newTrans;
}

FVector UWeArtDeviceTrackingObject::GetLocationOffset() const
{
	return LocationOffset;
}

FRotator UWeArtDeviceTrackingObject::GetRotationOffset() const
{
	return RotationOffset;
}

#if WITH_EDITOR
void UWeArtDeviceTrackingObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
    
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UWeArtDeviceTrackingObject, OffsetPreset))
	{
		SetOffsetValues(offsetPresetsMap[OffsetPreset].first, offsetPresetsMap[OffsetPreset].second);
	}
}

void UWeArtDeviceTrackingObject::PostLoad()
{
	Super::PostLoad();

	SetOffsetValues(offsetPresetsMap[OffsetPreset].first, offsetPresetsMap[OffsetPreset].second);
}
#endif

void UWeArtDeviceTrackingObject::SetOffsetValues(const FVector& OffsetPosition, const FRotator& OffsetRotation)
{
	LocationOffset = OffsetPosition;
	RotationOffset = OffsetRotation;
}
