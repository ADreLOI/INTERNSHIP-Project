// Fill out your copyright notice in the Description page of Project Settings.


#include "WeArtHapticController.h"


#include "DrawDebugHelpers.h"
#include "weArtPlugin.h"
#include "WeArtTouchEffect.h"
#include "WeArtUtilities.h"

UWeArtHapticController::UWeArtHapticController(): IndexPoint{}, MiddlePoint{}, AnnularPoint{}, PinkyPoint{}, ThumbPoint{},
                                                  PalmPoint{},
                                                  DebugEffects(false),
                                                  DebugSurfaceExploration(false),
                                                  SurfaceExplorationDebugColor(FColor::Green), HandController(nullptr),
                                                  PhysicHandler(nullptr),
                                                  LastIndexTime(0), WeArtControllerInstance(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UWeArtHapticController::BeginPlay()
{
	Super::BeginPlay();

	WeArtControllerInstance = GetOuter()->GetWorld()->GetGameInstance()->GetSubsystem<UWeArtController>();
	
	HandController = Cast<UWeArtHandController>(GetOwner()->GetComponentByClass(UWeArtHandController::StaticClass()));
	PhysicHandler = Cast<UWeArtPhysicHandler>(GetOwner()->GetComponentByClass(UWeArtPhysicHandler::StaticClass()));
}

UWeArtTouchableObject* UWeArtHapticController::DefineFingerState(EWeArtFingerType FingerType, float& CurrentFingerClosure,
                                            float& ContactClosure) const
{
	switch (FingerType)
	{
		case EWeArtFingerType::Index:
			CurrentFingerClosure = HandController->GetIndexTrackingObject()->GetClosure();
			ContactClosure = HandController->IndexState.ContactClosure;
			
			return HandController->TouchedObjectByIndex == nullptr
				                ? HandController->IndexOverlapObj
				                : HandController->TouchedObjectByIndex;
		case EWeArtFingerType::Middle:
			CurrentFingerClosure = HandController->GetMiddleTrackingObject()->GetClosure();
			ContactClosure = HandController->MiddleState.ContactClosure;
			
			return HandController->TouchedObjectByMiddle == nullptr
				                ? HandController->MiddleOverlapObj
				                : HandController->TouchedObjectByMiddle;
		case EWeArtFingerType::Thumb:
			CurrentFingerClosure = HandController->GetThumbTrackingObject()->GetClosure();
			ContactClosure = HandController->ThumbState.ContactClosure;
	
			return HandController->TouchedObjectByThumb == nullptr
				                ? HandController->ThumbOverlapObj
				                : HandController->TouchedObjectByThumb;
		case EWeArtFingerType::Ring:
			CurrentFingerClosure = HandController->GetRingTrackingObject()->GetClosure();
			ContactClosure = HandController->RingState.ContactClosure;
		
			return HandController->TouchedObjectByRing == nullptr
								? HandController->RingOverlapObj
								: HandController->TouchedObjectByRing;
		case EWeArtFingerType::Pinky:
			CurrentFingerClosure = HandController->GetPinkyTrackingObject()->GetClosure();
			ContactClosure = HandController->PinkyState.ContactClosure;
			
			return HandController->TouchedObjectByPinky == nullptr
								? HandController->PinkyOverlapObj
								: HandController->TouchedObjectByPinky;
		default:
			CurrentFingerClosure = 0.f;
			ContactClosure = 0.f;
			
			return HandController->TouchedObjectByPalm == nullptr
								? HandController->PalmOverlapObj
								: HandController->TouchedObjectByPalm;
	}
}

void UWeArtHapticController::SetForceByFactor(const FName& TipSocketName, EWeArtFingerType FingerType)
{
	const FUFingerState FingerState = HandController->GetStateByType(FingerType);
	DynamicForceByDistance = PhysicHandler->GetDistanceBetweenPhysAndPhantomTips(TipSocketName, FingerForceRayColor, DebugFingerForceDistance) * ForceCoefficient;
	
	if (FingerState.CurrentClosure <= FingerState.ContactClosure && HandController->GrabbedObject)
	{
		DynamicForceByDistance = 0;
	}
}

void UWeArtHapticController::WriteForceToTouchEffect(UWeArtTouchEffect* TouchEffect) const
{
	TouchEffect->effForce.value = HandController->AnchoredTouchedObject
		? FMath::Clamp(TouchEffect->effForce.value, WeArtConstants::minInteractionForce, 1.f)
		: FMath::Clamp(TouchEffect->effForce.value, 0.f, 1.f);
}

void UWeArtHapticController::LogEffects(const UWeArtTouchEffect* TouchEffect, const UWeArtHapticObject* HapticObject) const
{
	FString ResultString = "NULL";
	FColor MessageColor = FColor::Red;
	if (TouchEffect)
	{
		FString TemperatureValue = FString::SanitizeFloat(TouchEffect->effTemperature.value);
		FString ForceValue = FString::SanitizeFloat(TouchEffect->effForce.value);
		FString TextureValue = FString::SanitizeFloat(TouchEffect->effTexture.textureVelocity);
	
		FString DisplayedTemperature = FString("Temperature: ").Append(TemperatureValue);
		FString DisplayedForce = FString("Force: ").Append(ForceValue);
		FString DisplayedTexture = FString("Texture: ").Append(TextureValue);

		FString ActuationPoint = WeArtUtilities::EnumToFString(static_cast<EActuationPoint>(HapticObject->GetActuationPoint()));

		ResultString = ActuationPoint + " " + DisplayedTemperature + " " + DisplayedForce + " " + DisplayedTexture;
		MessageColor = FColor::Yellow;
	}
	
	GEngine->AddOnScreenDebugMessage((uint64)GetUniqueID() + HapticObject->GetActuationPoint(), -1, MessageColor, ResultString);
}

void UWeArtHapticController::SetFingerForce(UWeArtHapticObject* HapticObject, UWeArtTouchEffect* TouchEffect,
                                            EWeArtFingerType FingerType, const FVector& LastLoc, float LastTime, UWeArtTouchableObject* ExploredTouchableObject)
{
	if (TouchEffect == nullptr)
	{
		HapticObject->ClearEffects();
		return;
	}
	
	float CurrentFingerClosure;
	float ContactClosure;

	UWeArtTouchableObject* TouchedObject = DefineFingerState(FingerType, CurrentFingerClosure, ContactClosure);
	
	if (!TouchedObject && ExploredTouchableObject && ExploredTouchableObject->GetIsSurfaceExplorationEnabled())
	{
		TouchedObject = ExploredTouchableObject;
	}

	
	SetForceByFactor(GetTipSocketByType(FingerType), FingerType);
	float MaxDistanceCovered = WeArtConstants::MaxDistanceForMinStiffness + (WeArtConstants::MaxDistanceForMaxStiffness - WeArtConstants::MaxDistanceForMinStiffness) * TouchedObject->GetStiffnessValue();

	TouchEffect->effForce.value = TouchEffect->effForce.DisableDynamicForce
		? TouchedObject->GetStiffnessValue()
		: FMath::Clamp(MaxDistanceCovered ? DynamicForceByDistance / MaxDistanceCovered : 0, 0.2f, 1.f);
	
	WriteForceToTouchEffect(TouchEffect);

	ApplyTextureVelocity(TouchEffect, TouchedObject, LastLoc, LastTime);
	
	if (!HapticObject->ContainsEffect(TouchEffect))
	{
		HapticObject->AddEffect(TouchEffect);
	}
	
	HapticObject->UpdateEffects();
}

void UWeArtHapticController::ApplyFingerEffect(UWeArtTouchableObject* ContactObject, const UWeArtTouchableObject* OverlapObject, UWeArtHapticObject* HapticObject, const FVector& LastPos, float LastTime, EWeArtFingerType FingerType)
{
	if (ContactObject || OverlapObject)
	{
		UWeArtTouchEffect* const tempEffect = ContactObject == nullptr
			                        ? OverlapObject->GetTouchEffect()
			                        : ContactObject->GetTouchEffect();

		if (ContactObject && (OverlapObject && OverlapObject != ContactObject))
		{
			tempEffect->effTemperature = OverlapObject->GetTouchEffect()->effTemperature;
		}
		else if (ContactObject && ContactObject->GetTouchEffect() && !OverlapObject)
		{
			ContactObject->GetTouchEffect()->effTemperature.value = ContactObject->GetTemperatureValue();
		}
		SetFingerForce(HapticObject, tempEffect, FingerType, LastPos,
					   LastTime, ContactObject);

		if (DebugEffects)
		{
			LogEffects(tempEffect, HapticObject);
		}
	}
	else
	{
		HapticObject->ClearEffects();
		HapticObject->UpdateEffects();

		if (DebugEffects)
		{
			LogEffects(nullptr, HapticObject);
		}
	}
}

bool UWeArtHapticController::HitObjectFromSocket(const FName& TraceSocket) const
{
	const auto PhysicalHand = PhysicHandler->PhysicalHand;

	const auto SocketRotation = PhysicalHand->GetBoneQuaternion(PhysicalHand->GetBoneName(0));
	const auto SocketLocation = PhysicalHand->GetSocketLocation(TraceSocket);
	const auto EndPosition = -SocketRotation.GetUpVector() * SurfaceExplorationLineLength;

	const auto HitActor = HandController->TraceRay(SocketLocation, EndPosition, false).GetActor();
	
	// Actor has touchable object and surface exploration flag is enabled
	if (const auto TouchableObject = HitActor != nullptr
		                                 ? HitActor->FindComponentByClass<UWeArtTouchableObject>()
		                                 : nullptr)
	{
		return TouchableObject->GetIsSurfaceExplorationEnabled();
	}

	// No touchable object
	return false;	
}

bool UWeArtHapticController::IsEnoughHitForExploration() const
{
	TArray<bool> Hits;

	TFunction<void(const FName&)> AddHitFromSocket = [&](const FName& SocketName)
	{
		if (const auto Hit = HitObjectFromSocket(SocketName))
		{
			Hits.Add(Hit);
		}
	};

	AddHitFromSocket(IndexPoint.FingerSocket);
	AddHitFromSocket(MiddlePoint.FingerSocket);
	AddHitFromSocket(ThumbPoint.FingerSocket);

	if (WeArtControllerInstance->DeviceGeneration == EDeviceGeneration::TouchDiverPro)
	{
		AddHitFromSocket(AnnularPoint.FingerSocket);
		AddHitFromSocket(PinkyPoint.FingerSocket);
		AddHitFromSocket(PalmPoint.FingerSocket);
	}

	return Hits.Num() > SURFACE_EXPLORATION_HITS;
}

void UWeArtHapticController::DrawSurfaceExploration(const FQuat& SocketRotation, const FVector& SocketLocation, float CapsuleRadius, float CapsuleHeight) const
{
	DrawDebugCapsule(GetWorld(),
	                 SocketLocation,
	                 CapsuleHeight,      
	                 CapsuleRadius,      
	                 SocketRotation,
	                 SurfaceExplorationDebugColor,
	                 false,  
	                 .01f   
	);
}

void UWeArtHapticController::ApplySurfaceExplorationFingerEffect(
    UWeArtHapticObject* WeArtHapticObject, const UWeArtTouchableObject* OverlapObj, const FName& TraceSocket, 
    const FVector& LastPosition, float LastTime, EWeArtFingerType FingerType, float CapsuleRadius, float CapsuleHeight)
{
    const auto PhysicalHand = PhysicHandler->PhysicalHand;
    const auto SocketRotation = PhysicalHand->GetSocketQuaternion(TraceSocket);
    const auto SocketLocation = PhysicalHand->GetSocketLocation(TraceSocket);

    TArray<FHitResult> CapsuleHits;

    if (GetWorld()->SweepMultiByObjectType(CapsuleHits,
                                           SocketLocation,
                                           SocketLocation,
                                           SocketRotation,
                                           FCollisionObjectQueryParams::AllObjects,
                                           FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHeight)))
    {
    	// Process each hit result
    	for (const FHitResult& Hit : CapsuleHits)
    	{
    		AActor* HitActor = Hit.GetActor();
    		if (!HitActor)
    		{
    			continue;
    		}

    		UWeArtTouchableObject* TouchableObject = HitActor->FindComponentByClass<UWeArtTouchableObject>();
    		if (!TouchableObject)
    		{
    			continue;
    		}

    		if (DebugSurfaceExploration)
    		{
    			GEngine->AddOnScreenDebugMessage((uint64)GetUniqueID() + 1, 1.f, SurfaceExplorationDebugColor, TouchableObject->GetOwner()->GetName());
    		}

    		ApplyFingerEffect(TouchableObject, OverlapObj, WeArtHapticObject, LastPosition, LastTime, FingerType);
    		break;
    	}
    }

    if (DebugSurfaceExploration)
    {
        DrawSurfaceExploration(SocketRotation, SocketLocation, CapsuleRadius, CapsuleHeight);
    }
}


void UWeArtHapticController::UpdateEffects()
{
	if (HandController == nullptr) return;

	bool TouchDiverHapticsValid = 
		HandController->GetMiddleHapticObject() &&
		HandController->GetIndexHapticObject() &&
		HandController->GetThumbHapticObject();

	if (WeArtControllerInstance->DeviceGeneration == EDeviceGeneration::TouchDiverPro)
	{
		TouchDiverHapticsValid = 
			TouchDiverHapticsValid && 
			HandController->GetRingHapticObject() &&
			HandController->GetPinkyHapticObject() &&
			HandController->GetPalmHapticObject();
	}

	if (!TouchDiverHapticsValid) return;

	FVector lastPosition = LastIndexPosition;
	float lastTime = LastIndexTime;
	
	LastIndexPosition = PhysicHandler->PhysicalHand->GetComponentLocation();
	LastIndexTime = GetWorld()->GetTimeSeconds();

	bool EnableSurfaceExploration = IsEnoughHitForExploration();
	
	if (EnableSurfaceExploration)
	{
		ApplySurfaceExplorationFingerEffect(HandController->GetIndexHapticObject(), HandController->IndexOverlapObj, IndexPoint.FingerSocket, lastPosition, lastTime, EWeArtFingerType::Index, IndexPoint.CapsuleRadius, IndexPoint.CapsuleHeight);
		ApplySurfaceExplorationFingerEffect(HandController->GetMiddleHapticObject(), HandController->MiddleOverlapObj, MiddlePoint.FingerSocket, lastPosition, lastTime, EWeArtFingerType::Middle, MiddlePoint.CapsuleRadius, MiddlePoint.CapsuleHeight);
		ApplySurfaceExplorationFingerEffect(HandController->GetThumbHapticObject(), HandController->ThumbOverlapObj, ThumbPoint.FingerSocket, lastPosition, lastTime, EWeArtFingerType::Thumb, ThumbPoint.CapsuleRadius, ThumbPoint.CapsuleHeight);

		if (WeArtControllerInstance->DeviceGeneration == EDeviceGeneration::TouchDiverPro)
		{
			ApplySurfaceExplorationFingerEffect(HandController->GetRingHapticObject(), HandController->RingOverlapObj, AnnularPoint.FingerSocket, lastPosition, lastTime, EWeArtFingerType::Ring, AnnularPoint.CapsuleRadius, AnnularPoint.CapsuleHeight);
			ApplySurfaceExplorationFingerEffect(HandController->GetPinkyHapticObject(), HandController->PinkyOverlapObj, PinkyPoint.FingerSocket, lastPosition, lastTime, EWeArtFingerType::Pinky, PinkyPoint.CapsuleRadius, PinkyPoint.CapsuleHeight);
			ApplySurfaceExplorationFingerEffect(HandController->GetPalmHapticObject(), HandController->PalmOverlapObj, PalmPoint.FingerSocket, lastPosition, lastTime, EWeArtFingerType::Palm, PalmPoint.CapsuleRadius, PalmPoint.CapsuleHeight);
		}
	}
	else
	{

		ApplyFingerEffect(HandController->TouchedObjectByIndex, HandController->IndexOverlapObj, HandController->GetIndexHapticObject(), lastPosition, lastTime, EWeArtFingerType::Index);
		ApplyFingerEffect(HandController->TouchedObjectByMiddle, HandController->MiddleOverlapObj, HandController->GetMiddleHapticObject(), lastPosition, lastTime, EWeArtFingerType::Middle);
		ApplyFingerEffect(HandController->TouchedObjectByThumb, HandController->ThumbOverlapObj, HandController->GetThumbHapticObject(), lastPosition, lastTime, EWeArtFingerType::Thumb);

		if (WeArtControllerInstance->DeviceGeneration == EDeviceGeneration::TouchDiverPro)
		{
			ApplyFingerEffect(HandController->TouchedObjectByRing, HandController->RingOverlapObj, HandController->GetRingHapticObject(), lastPosition, lastTime, EWeArtFingerType::Ring);
			ApplyFingerEffect(HandController->TouchedObjectByPinky, HandController->PinkyOverlapObj, HandController->GetPinkyHapticObject(), lastPosition, lastTime, EWeArtFingerType::Pinky);
			ApplyFingerEffect(HandController->TouchedObjectByPalm, HandController->PalmOverlapObj, HandController->GetPalmHapticObject(), lastPosition, lastTime, EWeArtFingerType::Palm);
		}
	}
}

/*
* 1 - if the texture is not active we do not apply any texture value
* 2 - if the texture is active, but without forced velocity, we apply texture change during sliding on the surface, but if we do not grab it.
* 3 - if the texture is enabled with forced velocity, we apply fixed texture value during touch, slide and grab
*/
void UWeArtHapticController::ApplyTextureVelocity(UWeArtTouchEffect* TouchEffect, const UWeArtTouchableObject* TouchableObject, const FVector& LastLoc, float LastTime) const
{
	float dx = FVector::Distance(LastIndexPosition, LastLoc) / 100;
	float dt = FMath::Max<float>(FLT_EPSILON, LastIndexTime - LastTime);
	float slidingSpeed = (dx / dt) * 10;
	
	if (TouchableObject)
	{
		if ( (HandController->GrabbedObject || HandController->AnchoredTouchedObject) && !TouchEffect->effTexture.isForcedTextureVelocity)
		{
			TouchEffect->effTexture.textureVelocity = 0;
			return;
		}
		
		TouchEffect->effTexture.textureVelocity = !TouchEffect->effTexture.isForcedTextureVelocity ? slidingSpeed : WeArtConstants::staticTextureVelocity;
	}
}

// Called every frame
void UWeArtHapticController::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateEffects();
}


FName UWeArtHapticController::GetTipSocketByType(EWeArtFingerType FingerType) const
{
	switch (FingerType)
	{
	case EWeArtFingerType::Index:
		return FName("SocketIndex");
	case EWeArtFingerType::Thumb:
		return FName("SocketThumb");
	case EWeArtFingerType::Middle:
		return FName("SocketMiddle");
	case EWeArtFingerType::Ring:
		return FName("SocketRing");
	case EWeArtFingerType::Pinky:
		return FName("SocketPinky");
	case EWeArtFingerType::Palm:
		return FName("SocketPalmSfExp");
	default:
		return FName("Invalid Finger Type");
	}
}
