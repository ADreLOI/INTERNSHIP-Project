// Fill out your copyright notice in the Description page of Project Settings.


#include "WeArtHandController.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "WeArtContactPoint.h"
#include "WeArtContactPointPartType.h"
#include "WeArtController.h"
#include "WeArtDebugActor.h"
#include "WeArtFingersDictionary.h"
#include "WeArtGraspingManager.h"
#include "Components/CapsuleComponent.h"
#include "WeArtMessages.h"
#include "WeArtPhysicsInteractionComponent.h"
#include "weArtPlugin.h"
#include "WeArtTouchableObject.h"
#include "WeArtUtilities.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Math/UnitConversion.h"

UWeArtHapticObject* UWeArtHandController::GetThumbHapticObject() const
{
	return ThumbHapticObject;
}

UWeArtHapticObject* UWeArtHandController::GetIndexHapticObject() const
{
	return IndexHapticObject;
}

UWeArtHapticObject* UWeArtHandController::GetMiddleHapticObject() const
{
	return MiddleHapticObject;
}

UWeArtHapticObject* UWeArtHandController::GetRingHapticObject() const
{
	return  RingHapticObject;
}

UWeArtHapticObject* UWeArtHandController::GetPinkyHapticObject() const
{
	return PinkyHapticObject;
}

UWeArtHapticObject* UWeArtHandController::GetPalmHapticObject() const
{
	return PalmHapticObject;
}

// Sets default values for this component's properties
UWeArtHandController::UWeArtHandController(): WeArtController(nullptr), EHandSide(), GrabbedObject(nullptr),
                                              TouchedObject(nullptr),
                                              AnchoredTouchedObject(nullptr),
                                              TouchedObjectByThumb(nullptr),
                                              TouchedObjectByIndex(nullptr),
                                              TouchedObjectByMiddle(nullptr),
                                              TouchedObjectByRing(nullptr),
                                              TouchedObjectByPinky(nullptr),
                                              TouchedObjectByPalm(nullptr), IndexState(),
                                              MiddleState(), ThumbState(),
                                              PalmState(), RingState(),
                                              PinkyState(),
                                              DebugClosureValues(false), DebugGraspConditions(false),
                                              WeArtGraspEvents(nullptr),
                                              PalmOverlapObj(nullptr),
                                              RingOverlapObj(nullptr),
                                              PinkyOverlapObj(nullptr),
                                              IndexOverlapObj(nullptr),
                                              MiddleOverlapObj(nullptr),
                                              ThumbOverlapObj(nullptr),
                                              AnimInstance(nullptr), PhysicalHand(nullptr),
                                              VirtualHand(nullptr),
                                              VirtualHandAnim(nullptr),
                                              DebugActor(nullptr),
                                              ConditionA(false),
                                              ConditionB(false),
                                              ConditionC(false),
                                              ThumbHapticObject(nullptr),
                                              IndexHapticObject(nullptr),
                                              MiddleHapticObject(nullptr),
                                              RingHapticObject(nullptr),
                                              PinkyHapticObject(nullptr),
                                              PalmHapticObject(nullptr),
                                              bFoundGraspEvents(false),
                                              ThumbTrackingObject(nullptr),
                                              IndexTrackingObject(nullptr),
                                              MiddleTrackingObject(nullptr),
                                              RingTrackingObject(nullptr),
                                              PinkyTrackingObject(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UWeArtHandController::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeArtGraspEvents::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		bFoundGraspEvents = true;
		WeArtGraspEvents = Cast<AWeArtGraspEvents>(FoundActors[0]);
	}
}

void UWeArtHandController::InitCommonComponents()
{
	PhysicalHand = Cast<USkeletalMeshComponent>(
			GetOwner()->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), PhysicalHandTag)[0]);
	VirtualHand = Cast<USkeletalMeshComponent>(
		GetOwner()->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), VirtualHandTag)[0]);
	AnimInstance = Cast<UWeArtHandAnimInstance>(PhysicalHand->GetAnimInstance());
	VirtualHandAnim = Cast<UWeArtVirtualHand>(VirtualHand->GetAnimInstance());

	for (TActorIterator<AWeArtDebugActor> ActorItr(GetWorld(), AWeArtDebugActor::StaticClass()); ActorItr; ++ActorItr)
	{
		AWeArtDebugActor* TActor = *ActorItr;
		if (TActor != nullptr)
			DebugActor = TActor;
	}

	// Initialize closure and abduction values
	IndexState.MaxClosureByCalculation = 1;
	MiddleState.MaxClosureByCalculation = 1;
	ThumbState.MaxClosureByCalculation = 1;
	ThumbState.MaxAbdcationByCalculation = 1;
}

// Called during component initialization, initializes initial values for variables.
void UWeArtHandController::Init(UWeArtThimbleTrackingObject* ThumbTracking, UWeArtThimbleTrackingObject* IndexTracking,
                                UWeArtThimbleTrackingObject* MiddleTracking, UWeArtThimbleTrackingObject* RingTracking,
                                UWeArtThimbleTrackingObject* PinkyTracking,
                                UWeArtHapticObject* ThumbHaptic, UWeArtHapticObject* IndexHaptic,
                                UWeArtHapticObject* MiddleHaptic, UWeArtHapticObject* RingHaptic,
                                UWeArtHapticObject* PinkyHaptic, UWeArtHapticObject* PalmHaptic)
{

	WeArtController = GetOuter()->GetWorld()->GetGameInstance()->GetSubsystem<UWeArtController>();
	
	// Init haptic components
	ThumbHapticObject = ThumbHaptic;
	IndexHapticObject = IndexHaptic;
	MiddleHapticObject = MiddleHaptic;

	// Init tracking components
	ThumbTrackingObject = ThumbOutOfService ? IndexTracking : ThumbTracking;
	IndexTrackingObject = EmulateG2Fingers ? RingTracking : IndexTracking;
	MiddleTrackingObject = EmulateG2Fingers ? PinkyTracking : MiddleTracking;

	// Initialize common components
	InitCommonComponents();

	// Check initialization state
	bInitialized = ThumbHapticObject && IndexHapticObject && MiddleHapticObject &&
				   ThumbTrackingObject && IndexTrackingObject && MiddleTrackingObject;
	
    if (WeArtController->DeviceGeneration == EDeviceGeneration::TouchDiverPro)
    {
        RingHapticObject = RingHaptic;
        PinkyHapticObject = PinkyHaptic;

        RingTrackingObject = EmulateG2Fingers ? IndexTracking : RingTracking;
        PinkyTrackingObject = EmulateG2Fingers ? MiddleTracking : PinkyTracking;

		PalmHapticObject = PalmHaptic;
    	
        // Initialize closure values for Ring and Pinky fingers
        RingState.MaxClosureByCalculation = 1;
        PinkyState.MaxClosureByCalculation = 1;
	
        // Check initialization state
        bInitialized = bInitialized && RingHapticObject && PinkyHapticObject && PalmHapticObject && RingTrackingObject && PinkyTrackingObject;
    }

	FingersDictionary = {WeArtController->DeviceGeneration};
	
	if (!bInitialized)
	{
		UE_LOG(LogWeArt, Error, TEXT("Touch Diver%s initialization error!"),
		 WeArtController->DeviceGeneration == EDeviceGeneration::TouchDiverPro ? TEXT(" Pro") : TEXT(""));
	}
}

FHitResult UWeArtHandController::TraceRay(const FVector& StartLoc, const FVector& Direction, bool Debug = false) const
{
	FHitResult HitResult;
	
	const FVector EndLocation = StartLoc + (Direction * 2.f);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	FColor Color = FColor::Red;

	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLoc,
		EndLocation,
		ECC_Visibility,
		CollisionParams
	);
	
	if (Debug)
	{
		if (HitResult.GetComponent())
		{
			Color = FColor::Green;
		}
		
		DrawDebugLine(
			GetWorld(),
			StartLoc,
			EndLocation,
			Color,
			false,
			-1,
			0,
			0.1f
		);
	}
	
	return HitResult;
}


AActor* UWeArtHandController::TraceCapsuleOutActors(FVector Location, float Radius, FVector& HitLocation, FVector& HitNormal,
                                           TArray<AActor*>& OutActors, bool Debug) const
{
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	TArray<TEnumAsByte<EObjectTypeQuery>> ComponentTypesArray;
	
	TArray<AActor*> ActorsToIgnore;
	TArray<UPrimitiveComponent*> OutComponents;
	
	ActorsToIgnore.Add(GetOwner());
	ActorsToIgnore.Add(IndexHapticObject->GetOwner());
	ActorsToIgnore.Add(MiddleHapticObject->GetOwner());
	ActorsToIgnore.Add(ThumbHapticObject->GetOwner());
	
	if (WeArtController->DeviceGeneration == EDeviceGeneration::TouchDiverPro)
	{
		ActorsToIgnore.Add(RingHapticObject->GetOwner());
		ActorsToIgnore.Add(PinkyHapticObject->GetOwner());
		ActorsToIgnore.Add(PalmHapticObject->GetOwner());
	}
	
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));	
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	
	
	bool isComplete = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Location, Radius, ObjectTypesArray, nullptr,
		ActorsToIgnore, OutActors);

	bool isComponentFound = UKismetSystemLibrary::SphereOverlapComponents(GetWorld(), Location, Radius, ComponentTypesArray, nullptr, ActorsToIgnore,OutComponents);
	
	if (Debug)
		DrawDebugSphere(GetWorld(), Location, Radius, 10, FColor::Red, false, 0.001f);

	FVector shortestPoint = FVector(0,0,0);
	if (isComponentFound)
	{
		TArray<FVector> OutPoints;
		OutPoints.SetNum(OutComponents.Num());
		for (int32 i = 0; i < OutComponents.Num(); i++)
		{
			if (OutComponents[i])
			{
				OutComponents[i]->GetClosestPointOnCollision(Location, OutPoints[i]);
			}
		}
		
		shortestPoint = OutPoints[0];
	
		if (OutPoints.Num() > 0)
		{
			float shortest = FVector::Distance(OutPoints[0], Location);
	
			for (auto vector : OutPoints)
			{
				float distance = FVector::Distance(vector, Location);
				if (distance < shortest)
				{
					shortest = distance;
					shortestPoint = vector;
				}		
			}
		}
	}
	
	if (isComplete)
	{
		FVector OutPoint;

		if (auto MeshComponent = OutActors[0]->GetComponentByClass(UMeshComponent::StaticClass()))
		{
			Cast<UMeshComponent>(MeshComponent)->GetClosestPointOnCollision(Location, OutPoint);
		}

		if (isComponentFound)
		{
			HitLocation = shortestPoint;
		}
		else
		{
			HitLocation = OutPoint;
		}

		HitNormal = shortestPoint - Location;
		
		if (Debug)
		{
			FHitResult HitResult;
			DrawDebugLine(GetWorld(), Location, Location + HitNormal, FColor::Red);
			DrawDebugPoint(GetWorld(), HitResult.Location, 10, FColor::Orange, false, 1);
			DrawDebugSphere(GetWorld(), Location, Radius, 10, FColor::Green, false, 0.001f);
		}

		

		return OutActors[0];
	}
	HitNormal = FVector(0,0,0);
	HitLocation = FVector(0,0,0);
	return nullptr;
}

AActor* UWeArtHandController::TraceCapsule(FVector Location, float Radius, FVector& HitLocation, FVector& HitNormal,
										   bool Debug) const
{
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OutActors;

	ActorsToIgnore.Add(GetOwner());
	ActorsToIgnore.Add(IndexHapticObject->GetOwner());
	ActorsToIgnore.Add(MiddleHapticObject->GetOwner());
	ActorsToIgnore.Add(ThumbHapticObject->GetOwner());
	if (WeArtController->DeviceGeneration == EDeviceGeneration::TouchDiverPro)
	{
		ActorsToIgnore.Add(RingHapticObject->GetOwner());
		ActorsToIgnore.Add(PinkyHapticObject->GetOwner());
		ActorsToIgnore.Add(PalmHapticObject->GetOwner());
	}
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));	
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	
	
	bool isComplete = isComplete = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Location, Radius, ObjectTypesArray, nullptr,
		ActorsToIgnore, OutActors);
	
	if (Debug)
		DrawDebugSphere(GetWorld(), Location, Radius, 10, FColor::Red, false, 0.001f);

	if (isComplete)
	{
		FVector OutPoint;
		if (auto MeshComponent = OutActors[0]->GetComponentByClass(UMeshComponent::StaticClass()))
		{
			Cast<UMeshComponent>(MeshComponent)->GetClosestPointOnCollision(Location, OutPoint);
		}
		HitLocation = OutPoint;
		HitNormal = OutPoint - Location;

		if (Debug)
		{
			FHitResult HitResult;
			DrawDebugPoint(GetWorld(), HitResult.Location, 10, FColor::Orange, false, 1);
			DrawDebugSphere(GetWorld(), Location, Radius, 10, FColor::Green, false, 0.001f);
		}

		

		return OutActors[0];
	}
	HitNormal = FVector(0,0,0);
	HitLocation = FVector(0,0,0);
	return nullptr;
}

AActor* UWeArtHandController::TraceBox(FVector Location, FVector Extent, FRotator Orientation, bool Debug) const
{
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	if (Debug)
	{
		DrawDebugBox(
			GetWorld(),
			Location,
			Extent,
			Orientation.Quaternion(),
			FColor::Red,
			false,        
			0,            
			2.0f          
		);		
	}
	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	TArray<AActor*> ActorsToIgnore;
	
	ActorsToIgnore.Add(GetOwner());
	ActorsToIgnore.Add(IndexHapticObject->GetOwner());
	ActorsToIgnore.Add(MiddleHapticObject->GetOwner());
	ActorsToIgnore.Add(ThumbHapticObject->GetOwner());
	if (WeArtController->DeviceGeneration == EDeviceGeneration::TouchDiverPro)
	{
		ActorsToIgnore.Add(RingHapticObject->GetOwner());
		ActorsToIgnore.Add(PinkyHapticObject->GetOwner());
		ActorsToIgnore.Add(PalmHapticObject->GetOwner());
	}
	
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));	
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	
	FHitResult hit;
	if (UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), Location, Location, Extent,
	                                                                     Orientation, ObjectTypesArray, false,
	                                                                     ActorsToIgnore, EDrawDebugTrace::None, hit,
	                                                                     true))
	{
		if (Debug)
		{
			DrawDebugBox(
				GetWorld(),
				Location, Extent,
				Orientation.Quaternion(),
				FColor::Green,
				false,
				0,
				2.0f
			);
		}
		return hit.GetActor();
	}
	return nullptr;
}

const USkeletalMeshComponent* UWeArtHandController::GetPhysicalHand() const
{
	return PhysicalHand;
}

UWeArtThimbleTrackingObject* UWeArtHandController::GetThumbTrackingObject() const
{
	return ThumbTrackingObject;
}

UWeArtThimbleTrackingObject* UWeArtHandController::GetIndexTrackingObject() const
{
	return IndexTrackingObject;
}

UWeArtThimbleTrackingObject* UWeArtHandController::GetMiddleTrackingObject() const
{
	return MiddleTrackingObject;
}

UWeArtThimbleTrackingObject* UWeArtHandController::GetRingTrackingObject() const
{
	return RingTrackingObject;
}

UWeArtThimbleTrackingObject* UWeArtHandController::GetPinkyTrackingObject() const
{
	return PinkyTrackingObject;
}

FUFingerState UWeArtHandController::GetStateByType(EWeArtFingerType FingerType) const
{
	switch (FingerType)
	{
	case EWeArtFingerType::Index:
		return IndexState;
	case EWeArtFingerType::Middle:
		return MiddleState;
	case EWeArtFingerType::Thumb:
		return ThumbState;
	case EWeArtFingerType::Pinky:
		return PinkyState;
	case EWeArtFingerType::Ring:
		return RingState;
	default:
		return FUFingerState();
	}
}

// Called to update the finger compression limits for gripping.
void UWeArtHandController::UpdateClosureLimitsWhenGrabbed() const
{
	const auto VirtualAnim = Cast<UWeArtVirtualHand>(VirtualHand->GetAnimInstance());
	VirtualAnim->CheckSafeFingerClosure(EWeArtFingerType::Index, IndexState);
}

void UWeArtHandController::HandleFingerPoints()
{
	MasterContactPointsCount = 0;
	SlaveContactPointsCount = 0;
	ContactedPoints.Empty();
    
	HandleMasterPoints();
	HandleSlavePoints();
	
	UpdateFingerState(&IndexState, EWeArtFingerType::Index);
	UpdateFingerState(&MiddleState, EWeArtFingerType::Middle);
	UpdateFingerState(&ThumbState, EWeArtFingerType::Thumb);

	if (WeArtController->DeviceGeneration == EDeviceGeneration::TouchDiverPro)
	{
		UpdateFingerState(&RingState, EWeArtFingerType::Ring);
		UpdateFingerState(&PinkyState, EWeArtFingerType::Pinky);
		UpdateFingerState(&PalmState, EWeArtFingerType::Palm);
	}


	// if (GrabbedObject)
	// {
	// 	return;
	// }

	IndexState.CurrentClosure = IndexTrackingObject->GetClosure();
	MiddleState.CurrentClosure = MiddleTrackingObject->GetClosure();	

	if (WeArtController->DeviceGeneration == EDeviceGeneration::TouchDiverPro)
	{
		RingState.CurrentClosure = RingTrackingObject->GetClosure();
		PinkyState.CurrentClosure = PinkyTrackingObject->GetClosure();
	}
	
	ThumbState.CurrentClosure = ThumbTrackingObject->GetClosure();
	ThumbState.CurrentAbduction = ThumbTrackingObject->GetAbduction();
	
	AnimInstance->UpdateAnimation();
}


// The method is called when the fingers are updated, sets the finger value across all contact points.
void UWeArtHandController::UpdateFingerState(FUFingerState* FingerState, EWeArtFingerType FingerType)
{
	TArray<float> FingerContactClosure;
	TArray<float> FingerContactAbduction;
	TArray<bool> FingerContacted;
	TArray<bool> FingerInObjectRange;
	TArray<UWeArtTouchableObject*> FingerTouched;
	
	int32 MasterContactedNum = 0;
	int32 SlaveContactedNum = 0;

	for (auto MasterContact : FingersDictionary.WeArtFingersMasterDictionary)
	{
		if (MasterContact.Value.GetFingerType() == FingerType)
		{
			if (MasterContact.Value.IsContacted())
			{
				MasterContactedNum++;
			}
			FingerContactClosure.Push(MasterContact.Value.GetContactClosure());
			FingerContacted.Push(MasterContact.Value.IsContacted());
			FingerContactAbduction.Push(MasterContact.Value.GetContactAbduction());
			FingerTouched.Push(MasterContact.Value.GetTouchedObject());
			FingerInObjectRange.Push(MasterContact.Value.IsObjectInRadius());
		}
	}
	
	for (auto SlaveContact : FingersDictionary.WeArtFingersSlaveDictionary)
	{
		if (SlaveContact.Value.GetFingerType() == FingerType)
		{
			if (SlaveContact.Value.IsContacted())
			{
				SlaveContactedNum++;
			}
			FingerContactClosure.Push(SlaveContact.Value.GetContactClosure());
			FingerContacted.Push(SlaveContact.Value.IsContacted());
			FingerTouched.Push(SlaveContact.Value.GetTouchedObject());
			FingerContactAbduction.Push(SlaveContact.Value.GetContactAbduction());
			FingerInObjectRange.Push(SlaveContact.Value.IsObjectInRadius());
		}
	}

	bool TempCollided = false;
	bool TempInRange = false;
	UWeArtTouchableObject* TempTouchedObject = nullptr;

	for (int32 i = 0; i < FingerContacted.Num(); i++)
	{
		if (FingerContacted[i])
		{
			TempCollided = true;
		}
		if (FingerInObjectRange[i])
		{
			TempInRange = true;
		}
		if (FingerTouched[i] != nullptr)
		{
			TempTouchedObject = FingerTouched[i];
		}
	}

	auto Response = ECR_Ignore;

	if (TempTouchedObject)
	{
		Response = GetResponseToHand(TempTouchedObject->GetOwner());
	}

	if (!FingerState->Collided && TempCollided)
	{
		FingerState->ContactClosure = Response != ECR_Overlap
			? FMath::Clamp(FMath::Min(FingerContactClosure), 0.f,
				FingerState->MaxClosureByCalculation)
			: 1;

		FingerState->MaxAbdcation = GrabbedObject == nullptr || Response == ECR_Overlap
			                            ? GetMaxValue(FingerContactAbduction)
			                            : FMath::Clamp(GetMaxValue(FingerContactAbduction), 0.f,
			                                           FingerState->MaxAbdcationByCalculation);
	}

	if (!TempCollided)
	{
		FingerState->ContactClosure = !GrabbedObject ? 1 : FingerState->MaxClosureByCalculation;
	}
	
	FingerState->Collided = TempCollided;
	FingerState->ObjectInRange = TempInRange;
	
	switch (FingerType)
	{
		case EWeArtFingerType::Index: TouchedObjectByIndex = TempTouchedObject;
			break;
		case EWeArtFingerType::Middle: TouchedObjectByMiddle = TempTouchedObject;
			break;
		case EWeArtFingerType::Thumb: TouchedObjectByThumb = TempTouchedObject;
			break;
		case EWeArtFingerType::Ring: TouchedObjectByRing = TempTouchedObject;
			break;
		case EWeArtFingerType::Pinky: TouchedObjectByPinky = TempTouchedObject;
			break;
		case EWeArtFingerType::Palm: TouchedObjectByPalm = TempTouchedObject;
			break;
		default: TouchedObjectByThumb = TempTouchedObject;
	}

	MasterContactPointsCount += MasterContactedNum;
	SlaveContactPointsCount += SlaveContactedNum;
}

float UWeArtHandController::GetMaxValue(TArray<float> Numbers)
{
	float MaxValue = 1;
	bool AllZero = true;
	for (auto Value : Numbers)
	{
		if (Value != 0)
		{
			MaxValue = FMath::Min(MaxValue, Value);
			AllZero = false;
		}
	}
	return AllZero ? 0 : MaxValue;
}

bool UWeArtHandController::GetConditionA() const
{
	return ConditionA;
}

bool UWeArtHandController::GetConditionB() const
{
	return ConditionB;
}

bool UWeArtHandController::GetConditionC() const
{
	return ConditionC;
}

ECollisionResponse UWeArtHandController::GetResponseToHand(AActor* Source) const
{
	if (Source) 
	{
		ECollisionResponse OverlapReponse = Source->FindComponentByClass<UPrimitiveComponent>()->GetCollisionResponseToComponent(PhysicalHand);

		return OverlapReponse;
	} 

	return ECR_Ignore;
}

// Called to process contact points of type "master".
void UWeArtHandController::HandleMasterPoints()
{
	for (auto Element : FingersDictionary.WeArtFingersMasterDictionary)
	{
		FVector HitLocation, HitNormal;
		auto SocketName = Element.Value.GetContactSocketName();
		bool Contacted;
		float Closure = 0;
		float Abd = 0;
		TArray<AActor*> OutActors;
		TArray<AActor*> OutActorsFingerTips;
		AActor* ContactedActor = nullptr;
		
		switch (Element.Value.GetFingerType())
		{
			case EWeArtFingerType::Palm:
				ContactedActor = TraceBox(PhysicalHand->GetSocketLocation(FName(SocketName)), PalmBoxTraceExtent,
			                          PhysicalHand->GetSocketRotation(FName(SocketName)), false);
				OutActors.Add(ContactedActor);
				break;
			case EWeArtFingerType::Thumb:
				ContactedActor = TraceCapsuleOutActors(PhysicalHand->GetSocketLocation(FName(SocketName)), FingerTraceRadius,
			                              HitLocation,
			                              HitNormal,
			                              OutActors,
			                              false);
				Closure = AnimInstance->ThumbFingerAlpha;
				Abd = AnimInstance->ThumbFingerAbductionAlpha;
				break;
		}
		
		auto TempElem = Element.Value;
		auto TouchableObject = ContactedActor != nullptr
			                       ? ContactedActor->GetComponentByClass(UWeArtTouchableObject::StaticClass())
			                       : nullptr;
		
		Contacted = TouchableObject != nullptr;
		auto inRadius = TraceCapsuleOutActors(PhysicalHand->GetSocketLocation(FName(SocketName)), FingerTraceRadius * 2,
		                             HitLocation,
		                             HitNormal,
		                             OutActorsFingerTips,
		                             false) != nullptr;
		
		
		AActor* Overlaped = nullptr;
		ContactedActor = nullptr;
		
		for (auto seekActor : OutActors)
		{
			if (GetResponseToHand(seekActor) != ECR_Overlap)
			{
				ContactedActor = seekActor;
			}
			else
			{
				Overlaped = seekActor;
			}
		}

		inRadius = false;
		
		for (auto seekActor : OutActorsFingerTips)
		{
			if (GetResponseToHand(seekActor) != ECR_Overlap)
			{
				inRadius = true;
			}
		}
		
		if (Element.Key == WeArtContactPointPartType::Thumb && Overlaped)
		{
			ThumbOverlapObj = Cast<UWeArtTouchableObject>(Overlaped->FindComponentByClass<UWeArtTouchableObject>());
		}
		else if (Element.Key == WeArtContactPointPartType::Thumb && !Overlaped)
		{
			ThumbOverlapObj = nullptr;
		}

		if (Element.Key == WeArtContactPointPartType::Palm && Overlaped)
		{
			PalmOverlapObj = Cast<UWeArtTouchableObject>(Overlaped->FindComponentByClass<UWeArtTouchableObject>());
		}
		else if(Element.Key == WeArtContactPointPartType::Palm && !Overlaped)
		{
			PalmOverlapObj = nullptr;
		}
		
		Contacted = ContactedActor != nullptr;
		
		if (Contacted)
		{
			ContactedPoints.Push(Element.Key);
			if (Element.Value.GetContactPointType() == WeArtContactPointType::Master || Element.Value.
				GetContactPointType() == WeArtContactPointType::Slave)
			{
				TempElem.SetTouchedObject(
					Cast<UWeArtTouchableObject>(ContactedActor->FindComponentByClass<UWeArtTouchableObject>()));
			}
			
			if (!TempElem.IsContacted())
				TempElem.SetContactClosure(Closure);
			
		}
		else
		{
			TempElem.SetContactClosure(1);
			TempElem.SetTouchedObject(nullptr);
		}
		
		TempElem.SetContactAbduction(Abd);
		TempElem.SetObjectInRadius(inRadius);
		TempElem.SetIsContacted(Contacted);
		TempElem.SetContactLocation(HitLocation);
		TempElem.SetContactNormal(HitNormal);
		FingersDictionary.WeArtFingersMasterDictionary[Element.Key] = TempElem;
	}
}

// Called to process contact points of type "slave".
void UWeArtHandController::HandleSlavePoints()
{
	for (auto Element : FingersDictionary.WeArtFingersSlaveDictionary)
	{
		FVector HitLocation, HitNormal;
		auto SocketName = Element.Value.GetContactSocketName();
		bool Contacted;
		float Closure = 0;
		float Abd = 0;
		TArray<AActor*> OutActors;
		TArray<AActor*> OutActorsFingerTips;
		AActor* ContactedActor;

		switch (Element.Value.GetFingerType())
		{
		case EWeArtFingerType::Index:
			Closure = AnimInstance->IndexFingerAlpha;

			break;
		case EWeArtFingerType::Middle:
			Closure = AnimInstance->MiddleFingerAlpha;

			break;
		case EWeArtFingerType::Ring:
			Closure = AnimInstance->RingFingerAlpha;

			break;
		case EWeArtFingerType::Pinky:
			Closure = AnimInstance->PinkyFingerAlpha;
		
			break;
		
		default:
			Closure = AnimInstance->ThumbFingerAlpha;
			Abd = AnimInstance->ThumbFingerAbductionAlpha;
		}
		
		ContactedActor = TraceCapsuleOutActors(PhysicalHand->GetSocketLocation(FName(SocketName)), FingerTraceRadius,
		                              HitLocation,
		                              HitNormal,
		                              OutActors,
		                              false);
		auto inRadius = TraceCapsuleOutActors(PhysicalHand->GetSocketLocation(FName(SocketName)), FingerTraceRadius * 2,
		                             HitLocation,
		                             HitNormal,
		                             OutActorsFingerTips,
		                             false) != nullptr;
		auto TouchableObject = ContactedActor != nullptr
			                       ? ContactedActor->GetComponentByClass(UWeArtTouchableObject::StaticClass())
			                       : nullptr;
		
		Contacted = TouchableObject != nullptr;
		auto TempElem = Element.Value;
		
		AActor* Overlapped = nullptr;
		ContactedActor = nullptr;

		for (auto seekActor : OutActors)
		{
			if (GetResponseToHand(seekActor) != ECR_Overlap)
			{
				ContactedActor = seekActor;
			}
			else
			{
				Overlapped = seekActor;
			}
		}

		inRadius = false;
		
		for (auto seekActor : OutActorsFingerTips)
		{
			if (GetResponseToHand(seekActor) != ECR_Overlap)
			{
				inRadius = true;
			}
		}

		// Helper function to update the overlap object based on the finger part
		auto UpdateOverlapObject = [&](WeArtContactPointPartType PartType, UWeArtTouchableObject*& OverlapObj)
		{
			if (Element.Key == PartType)
			{
				OverlapObj = Overlapped ? Cast<UWeArtTouchableObject>(Overlapped->FindComponentByClass<UWeArtTouchableObject>()) : nullptr;
			}
		};

		// Update for TouchDiver type fingers
		UpdateOverlapObject(WeArtContactPointPartType::Index, IndexOverlapObj);
		UpdateOverlapObject(WeArtContactPointPartType::Middle, MiddleOverlapObj);

		// Check for additional fingers in TouchDiverPro mode
		if (WeArtController->DeviceGeneration == EDeviceGeneration::TouchDiverPro)
		{
			UpdateOverlapObject(WeArtContactPointPartType::Ring, RingOverlapObj);
			UpdateOverlapObject(WeArtContactPointPartType::Pinky, PinkyOverlapObj);
			UpdateOverlapObject(WeArtContactPointPartType::Palm, PalmOverlapObj);
		}
			
		Contacted = ContactedActor != nullptr;
		
		if (Contacted)
		{
			ContactedPoints.Push(Element.Key);
			TempElem.SetTouchedObject(
				Cast<UWeArtTouchableObject>(ContactedActor->FindComponentByClass<UWeArtTouchableObject>()));

			if (!TempElem.IsContacted())
				TempElem.SetContactClosure(Closure);
			
		}
		else
		{
			TempElem.SetContactClosure(1);
		    TempElem.SetTouchedObject(nullptr);
		}
		
		TempElem.SetObjectInRadius(inRadius);
		TempElem.SetIsContacted(Contacted);
		TempElem.SetContactAbduction(Abd);
		TempElem.SetContactLocation(HitLocation);
		TempElem.SetContactNormal(HitNormal);
		FingersDictionary.WeArtFingersSlaveDictionary[Element.Key] = TempElem;
	}
}

// Called every frame, provided the fingers are closed, checks if all the conditions for taking the object are met.
bool UWeArtHandController::CheckGrabableStatus()
{
	ConditionA = false;
	ConditionB = false;
	ConditionC = false;
	
	TArray<FVector> ContactLocations;
	TArray<FVector> ContactNormals;

	ConditionA = MasterContactPointsCount > 0 && SlaveContactPointsCount > 0 || MasterContactPointsCount >= 2;
	ConditionB = CheckHandClosingState();

	for (int32 i = 0; i < ContactedPoints.Num(); i++)
	{
		if (i + 1 < ContactedPoints.Num())
		{
			auto NormalOne = FingersDictionary.WeArtFingersMasterDictionary.Contains(ContactedPoints[i]) &&
			                 FingersDictionary.WeArtFingersMasterDictionary[ContactedPoints[i]].IsContacted()
				                 ? FingersDictionary.WeArtFingersMasterDictionary[ContactedPoints[i]].
				                 GetContactNormal()
				                 : FingersDictionary.WeArtFingersSlaveDictionary[ContactedPoints[i]].
				                 GetContactNormal();
			auto NormalTwo = FingersDictionary.WeArtFingersMasterDictionary.Contains(ContactedPoints[i + 1]) &&
			                 FingersDictionary.WeArtFingersMasterDictionary[ContactedPoints[i + 1]].IsContacted()
				                 ? FingersDictionary.WeArtFingersMasterDictionary[ContactedPoints[i + 1]].
				                 GetContactNormal()
				                 : FingersDictionary.WeArtFingersSlaveDictionary[ContactedPoints[i + 1]].
				                 GetContactNormal();
			if (bool result = FVector::DotProduct(NormalOne.GetSafeNormal(), NormalTwo.GetSafeNormal()) <
				ContactDirectionThreshold)
			{
				ConditionC = true;
				break;
			}
		}
	}

	const auto ThumbContactPoint = FingersDictionary.WeArtFingersMasterDictionary.Find(WeArtContactPointPartType::Thumb);
	if (!ConditionC && ThumbContactPoint && ThumbContactPoint->IsContacted())
	{
		if (ContactNormals.Num() > 1)
		{
			ConditionC = true;
		}
	}

	if (DebugGraspConditions)
	{
		FString DebugMsg = ConditionA ? "ConditionA:True\n" : "ConditionA:False\n";
		DebugMsg.Append(ConditionB ? "ConditionB:True\n" : "ConditionB:False\n");
		DebugMsg.Append(ConditionC ? "ConditionC:True\n" : "ConditionC:False\n");
		GEngine->AddOnScreenDebugMessage((uint64)GetUniqueID()+1, 1.f, FColor::Green, DebugMsg);
	}
	
	
	return ConditionA && ConditionB && ConditionC;
}

FRotator LookRotation(const FVector& direction, const FVector& up)
{
	// Create a rotation matrix that aligns the X-axis with the specified direction
	FMatrix RotMatrix = FRotationMatrix::MakeFromX(direction);

	// Convert the rotation matrix to a quaternion
	FQuat RotationQuat(RotMatrix);

	// Use the LookRotation method to adjust the rotation to the specified up direction
	FQuat FinalQuat = RotationQuat * FQuat::FindBetweenNormals(FVector::ForwardVector, up);

	// Convert the final quaternion to a rotator
	return FinalQuat.Rotator();
}

FVector CalculateForwardEndpoint(const FVector& StartPosition, const FVector& ForwardVector, float ExtensionLength)
{
	return StartPosition + ForwardVector * ExtensionLength;
}

UWeArtTouchableObject* UWeArtHandController::GetTouchedObjectByFinger(const FVector& Location) const
{
	FVector HitLocation;
	FVector HitNormal;
	auto TracedActor = TraceCapsule(Location, FingerTraceRadius * 1.2, HitLocation, HitNormal);
	if (TracedActor != nullptr)
	{
		return Cast<UWeArtTouchableObject>(TracedActor->GetComponentByClass(UWeArtTouchableObject::StaticClass()));
	}
	return nullptr;
}

bool UWeArtHandController::CheckHandClosingState()
{
	if (!WeArtController) return false;
	
	TFunction<bool(float, float, float)> CheckFingerClosure = [](float CurrentClosure, float ContactClosure, float Threshold)
	{
		return CurrentClosure != 0 && (CurrentClosure - ContactClosure) > Threshold;
	};
	
    // Common finger closures
    const float CurrentIndexClosure = IndexTrackingObject->GetClosure();
    const float CurrentMiddleClosure = MiddleTrackingObject->GetClosure();
    const float CurrentThumbClosure = ThumbTrackingObject->GetClosure();

	uint64 DisplayID = GetOwner()->GetUniqueID();
	FColor LogColor = FColor::Green;

    FString DebuggingClosures = (EHandSide == EHandSide::Right ? FString("RIGHT!!!\n") : FString("LEFT!!!\n")).Append(FString("Index: "))
                                                                                                              .Append(FString::SanitizeFloat(CurrentIndexClosure))
                                                                                                              .Append(" Middle:")
                                                                                                              .Append(FString::SanitizeFloat(CurrentMiddleClosure))
                                                                                                              .Append(" Thumb:").Append(FString::SanitizeFloat(CurrentThumbClosure).Append(" Abduction: ").Append(FString::SanitizeFloat(ThumbTrackingObject->GetAbduction())));
	
    // Check closure states for Index, Middle, and Thumb
    const bool ClosureStateIndex = CheckFingerClosure(CurrentIndexClosure, IndexState.ContactClosure, FingerCloseThreshold);
    const bool ClosureStateMiddle = CheckFingerClosure(CurrentMiddleClosure, MiddleState.ContactClosure, FingerCloseThreshold);
    const bool ClosureStateThumb = CheckFingerClosure(CurrentThumbClosure, ThumbState.ContactClosure, FingerCloseThreshold / 2);
	
    // Check collision states
    const bool CollidedIndex = IndexState.Collided;
    const bool CollidedMiddle = MiddleState.Collided;
    const bool CollidedThumb = ThumbState.Collided;
	const auto PalmContactPoint = FingersDictionary.WeArtFingersMasterDictionary.Find(WeArtContactPointPartType::Palm);
	const bool CollidedPalm = PalmContactPoint ? PalmContactPoint->IsContacted() : false;
	
    // Default closure check logic
    bool bIsClosed = (
        (ClosureStateIndex && ClosureStateThumb && CollidedIndex && CollidedThumb) ||
        (ClosureStateThumb && ClosureStateMiddle && CollidedMiddle && CollidedThumb) ||
        (ClosureStateIndex && CollidedPalm && CollidedIndex && AnimInstance->IndexFingerAlpha > ClosureThreshold) ||
        (ClosureStateMiddle && CollidedPalm && CollidedMiddle && AnimInstance->MiddleFingerAlpha > ClosureThreshold) ||
        (ClosureStateThumb && CollidedPalm && CollidedThumb && AnimInstance->ThumbFingerAlpha > ClosureThreshold)
    );

    // Additional check for TouchDiverPro
    if (WeArtController->DeviceGeneration == EDeviceGeneration::TouchDiverPro)
    {
        // Additional fingers for TouchDiverPro
        const float CurrentRingClosure = RingTrackingObject->GetClosure();
        const float CurrentPinkyClosure = PinkyTrackingObject->GetClosure();

        // Check closure states for Ring and Pinky
        const bool ClosureStateRing = CheckFingerClosure(CurrentRingClosure, RingState.ContactClosure, FingerCloseThreshold);
        const bool ClosureStatePinky = CheckFingerClosure(CurrentPinkyClosure, PinkyState.ContactClosure, FingerCloseThreshold);

        // Check collision states for Ring and Pinky
        const bool CollidedRing = RingState.Collided;
        const bool CollidedPinky = PinkyState.Collided;

        // Extended closure check logic for TouchDiverPro
        bIsClosed = bIsClosed || 
            (ClosureStateRing && ClosureStatePinky && CollidedRing && CollidedPinky) ||
            (ClosureStateRing  && CollidedRing && AnimInstance->RingFingerAlpha > ClosureThreshold) ||
            (ClosureStatePinky && CollidedPinky && AnimInstance->PinkyFingerAlpha > ClosureThreshold);

    	DebuggingClosures.Append(FString(" Ring: "))
			.Append(FString::SanitizeFloat(CurrentRingClosure))
			.Append(" Pinky:")
			.Append(FString::SanitizeFloat(CurrentPinkyClosure));
    }

	if (DebugClosureValues)
	{
		constexpr float TimeToDisplay = 0.1f;
		GEngine->AddOnScreenDebugMessage(DisplayID, TimeToDisplay, LogColor, DebuggingClosures);
	}
	
    // Update hand closing state
    ClosingState = bIsClosed ? HandClosingState::Closing : HandClosingState::Open;

    return bIsClosed;
}

void UWeArtHandController::UpdateEffectsAndFingerValues(const EDeviceGeneration DiverType)
{
	// Update current closure states for each finger
	IndexState.CurrentClosure = AnimInstance->IndexFingerAlpha;
	MiddleState.CurrentClosure = AnimInstance->MiddleFingerAlpha;
	ThumbState.CurrentClosure = AnimInstance->ThumbFingerAlpha;
	ThumbState.CurrentAbduction = AnimInstance->ThumbFingerAbductionAlpha;

	// Grab the touched object and update closure limits
	GrabbedObject = TouchedObject->GetOwner();
	UpdateClosureLimitsWhenGrabbed();
		
	if(DiverType == EDeviceGeneration::TouchDiverPro)
	{
		RingState.CurrentClosure = AnimInstance->RingFingerAlpha;		 // Added for ring finger
		PinkyState.CurrentClosure = AnimInstance->PinkyFingerAlpha;      // Added for pinky finger
	}
}

// The method handles the grab of an object.
void UWeArtHandController::HandleGraspingState(void)
{
	if (CanControllerGrab())
	{
		TouchedObject = GetTouchedObjectFromFingers();
		
		if (TouchedObject == nullptr)
		{
			return;
		}

		if (!TouchedObject->IsGraspable)
		{
			return;
		}
		
		if (bFoundGraspEvents)
		{
			WeArtGraspEvents->OnMiddlewareHandControllerGrasp.Broadcast(
				EHandSide, Cast<UWeArtTouchableObject>( TouchedObject));
		}
		
		GraspingState = GraspingState::Grabbed;
		
		UpdateEffectsAndFingerValues(WeArtController->DeviceGeneration);

		TouchedObject->Grab(GetOwner());	  
		
		// Avoid using standard Grab, if it's physics intractable object
		AnchoredTouchedObject = TouchedObject->GetOwner()->FindComponentByClass<UWeArtPhysicsInteractionComponent>();
		if (!AnchoredTouchedObject || !AnchoredTouchedObject->GetIsInteractionEnabled())
		{
			OnGrabbed.Broadcast();
		}
	}
	else if (GraspingState == GraspingState::Grabbed && ClosingState == Open)
	{
		if (bFoundGraspEvents)
		{
			WeArtGraspEvents->OnMiddlewareHandControllerRelease.Broadcast(EHandSide, TouchedObject);
		}
		
		OnReleasedGrabbedObject();
	}
}

UWeArtTouchableObject* UWeArtHandController::GetTouchedObjectFromFingers()
{
	if (!WeArtController) return nullptr;
	if (!IndexTrackingObject || !MiddleTrackingObject || !ThumbTrackingObject) return nullptr;
	if (!GetOwner()) return nullptr;
	
	const auto Actor = TraceBox(PhysicalHand->GetSocketLocation(FName("SocketCenter")), PalmBoxTraceExtent,
	                            PhysicalHand->GetSocketRotation(FName("SocketCenter")), false);
	const auto TouchedPalm = Actor == nullptr
		                         ? nullptr
		                         : Cast<UWeArtTouchableObject>(
			                         Actor->GetComponentByClass(UWeArtTouchableObject::StaticClass()));
	
	// Helper lambda function to check if a touched object matches other finger objects or palm
	auto IsMatchingTouchedObject = [&](const UWeArtTouchableObject* TouchedObject, const TArray<UWeArtTouchableObject*>& Fingers) -> bool
	{
		for (auto Finger : Fingers)
		{
			if (TouchedObject && Finger && TouchedObject == Finger)
			{
				return true;
			}
		}
		return false;
	};

	TArray<UWeArtTouchableObject*> ThumbMatchFingers {TouchedObjectByIndex, TouchedObjectByMiddle, TouchedPalm};
	TArray<UWeArtTouchableObject*> IndexMatchFingers = {TouchedObjectByThumb, TouchedObjectByIndex, TouchedPalm};
	TArray<UWeArtTouchableObject*> MiddleMatchFingers = {TouchedObjectByThumb, TouchedObjectByMiddle, TouchedPalm};
	
	if (WeArtController->DeviceGeneration == EDeviceGeneration::TouchDiverPro)
	{
		if (!RingTrackingObject || !PinkyTrackingObject) return nullptr;

		const TArray<UWeArtTouchableObject*> TouchDiverProUnits = {TouchedObjectByRing, TouchedObjectByPinky, TouchedObjectByPalm};
		ThumbMatchFingers.Append(TouchDiverProUnits);
		MiddleMatchFingers.Append(TouchDiverProUnits);
		IndexMatchFingers.Append(TouchDiverProUnits);

		if (IsMatchingTouchedObject(TouchedObjectByRing, {TouchedObjectByThumb, TouchedObjectByIndex, TouchedObjectByMiddle, TouchedObjectByPinky, TouchedPalm}))
		{
			return TouchedObjectByRing;
		}
		if (IsMatchingTouchedObject(TouchedObjectByPinky, {TouchedObjectByThumb, TouchedObjectByIndex, TouchedObjectByMiddle, TouchedObjectByRing, TouchedPalm}))
		{
			return TouchedObjectByPinky;
		}
	}

	// Check for TouchDiver fingers (Thumb, Index, Middle)
	if (IsMatchingTouchedObject(TouchedObjectByThumb, ThumbMatchFingers))
	{
		return TouchedObjectByThumb;
	}
	if (IsMatchingTouchedObject(TouchedObjectByIndex, IndexMatchFingers))
	{
		return TouchedObjectByIndex;
	}
	if (IsMatchingTouchedObject(TouchedObjectByMiddle, MiddleMatchFingers))
	{
		return TouchedObjectByMiddle;
	}
	
	// Default return to cover all paths
	return nullptr;
}

// The method is called when the user releases the object.
void UWeArtHandController::OnReleasedGrabbedObject()
{
	if (!GetOwner()) return;
	
	if (TouchedObject && IsValid(TouchedObject))
	{
		// Do not use detaching, if it's physics intractable object. Especially for child actor.
		AActor* TouchedObjectOwner = TouchedObject->GetOwner();
		if (TouchedObjectOwner && AnchoredTouchedObject && !AnchoredTouchedObject->GetIsInteractionEnabled()) 
		{
			TouchedObjectOwner->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
			

		TouchedObject->Release();
		TouchedObject = nullptr;
		OnReleased.Broadcast();
	}
		
	GrabbedObject = nullptr;
	IndexState.Allowed = false;
	MiddleState.Allowed = false;
	IndexState.Collided = false;
	MiddleState.Collided = false;
	
	IndexState.MinClosure = 0;
	MiddleState.MinClosure = 0;
	ThumbState.MinClosure = 0;
	ThumbState.MinAbdcation = 0;
	
	IndexState.MaxClosureByCalculation = 1;
	MiddleState.MaxClosureByCalculation = 1;
	ThumbState.MaxClosureByCalculation = 1;
	ThumbState.MaxAbdcationByCalculation = 1;

	if (WeArtController->DeviceGeneration == EDeviceGeneration::TouchDiverPro)
	{
		RingState.Allowed = false; 
		PinkyState.Allowed = false; 

		RingState.Collided = false; 
		PinkyState.Collided = false; 

		RingState.MinClosure = 0; 
		PinkyState.MinClosure = 0; 

		RingState.MaxClosureByCalculation = 1; 
		PinkyState.MaxClosureByCalculation = 1; 
	}

	OnGrabbed.Broadcast();
	GraspingState = GraspingState::Released;
	VirtualHandAnim->AbortVirtualHand();
}

// Called every frame
void UWeArtHandController::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bInitialized)
		return;
	CheckHandClosingState();
	HandleFingerPoints();
}

bool UWeArtHandController::CanControllerGrab()
{
	// Simplified condition only for physics interaction, to make grab more stable
	if (GetOwner() && AnchoredTouchedObject && AnchoredTouchedObject->GetIsInteractionEnabled())
	{
		return CheckGrabableStatus();
	}
	
	return GraspingState == GraspingState::Released && CheckGrabableStatus();

}
