// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "..\Common\WeArtFingerState.h"
#include "WeArtContactPoint.h"
#include "WeArtDebugActor.h"
#include "WeArtFingersDictionary.h"
#include "EWeArtFingerType.h"
#include "Components/ActorComponent.h"
#include "WeArtThimbleTrackingObject.h"
#include "WeArtHapticObject.h"
#include "WeArtTouchableObject.h"
#include "WeArtGraspEvents.h"
#include "WeArtHandAnimInstance.h"
#include "WeArtMessages.h"
#include "WeArtSettings.h"
#include "WeArtVirtualHand.h"

#include "WeArtHandController.generated.h"

class UWeArtPhysicsInteractionComponent;
/**
 * @class UWeArtHandController
 * @brief Controls hand interactions and haptic feedback.
 * 
 * This class manages hand interactions, such as grabbing objects and updating haptic feedback.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WEARTPLUGIN_API UWeArtHandController : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	UWeArtController* WeArtController;

public:
	/** Hand side enumeration */
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EHandSide> EHandSide;

	/** Pointer to the grabbed object */
	UPROPERTY(BlueprintReadOnly)
	AActor* GrabbedObject;
	
	/** Pointer to the touched object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWeArtTouchableObject* TouchedObject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWeArtPhysicsInteractionComponent* AnchoredTouchedObject;
	/** Pointer to the touched object by thumb */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWeArtTouchableObject* TouchedObjectByThumb;
	/** Pointer to the touched object by index finger */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWeArtTouchableObject* TouchedObjectByIndex;
	/** Pointer to the touched object by middle finger */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWeArtTouchableObject* TouchedObjectByMiddle;
	/** Pointer to the touched object by Ring finger */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWeArtTouchableObject* TouchedObjectByRing;
	/** Pointer to the touched object by pinky finger */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWeArtTouchableObject* TouchedObjectByPinky;
	/** Pointer to the touched object by palm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWeArtTouchableObject* TouchedObjectByPalm;
	/** Finger state variables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUFingerState IndexState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUFingerState MiddleState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUFingerState ThumbState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUFingerState PalmState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUFingerState RingState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUFingerState PinkyState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging | Device")
	bool EmulateG2Fingers = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging | Device")
	bool ThumbOutOfService = false;

	
	UPROPERTY(EditAnywhere, Category = "Debugging")
	bool DebugClosureValues;
	UPROPERTY(EditAnywhere, Category = "Debugging")
	bool DebugGraspConditions;
	UWeArtHandController();

	/** Event triggered when an object is grabbed */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrabbed);

	/** Event triggered when an object is released */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReleased);
	
	/** Event delegate for object grabbing */
	FOnGrabbed OnGrabbed;

	/** Event delegate for object releasing */
	FOnGrabbed OnReleased;
	
	/** Updates closure limits when an object is grabbed */
	void UpdateClosureLimitsWhenGrabbed() const;

	/** Handles releasing the grabbed object */
	void OnReleasedGrabbedObject();
	/** Handles grasping state */
	void HandleGraspingState(void);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool CanControllerGrab();
	
	/** Pointer to grasp events */
	UPROPERTY()
	AWeArtGraspEvents* WeArtGraspEvents;

	/** Returns the thumb haptic object */
	UWeArtHapticObject* GetThumbHapticObject() const;
	/** Returns the index finger haptic object */
	UWeArtHapticObject* GetIndexHapticObject() const;
	/** Returns the middle finger haptic object */
	UWeArtHapticObject* GetMiddleHapticObject() const;
	/** Returns the Ring finger haptic object */
	UWeArtHapticObject* GetRingHapticObject() const;
	/** Returns the pinky finger haptic object */
	UWeArtHapticObject* GetPinkyHapticObject() const;	/** Returns the pinky finger haptic object */
	/** Returns the palm haptic object */
	UWeArtHapticObject* GetPalmHapticObject() const;
	
	/** Gets the touched object from fingers */
	UWeArtTouchableObject* GetTouchedObjectFromFingers();
	/** Pointer to palm finger overlap objects to handle grabbing inside overlap volume */
	UPROPERTY()
	UWeArtTouchableObject* PalmOverlapObj;
	/** Pointer to ring finger overlap objects to handle grabbing inside overlap volume */
	UPROPERTY()
	UWeArtTouchableObject* RingOverlapObj;
	/** Pointer to pinky finger overlap objects to handle grabbing inside overlap volume */
	UPROPERTY()
	UWeArtTouchableObject* PinkyOverlapObj;
	/** Pointer to index finger overlap objects to handle grabbing inside overlap volume */
	UPROPERTY()
	UWeArtTouchableObject* IndexOverlapObj;
	/** Pointer to middle finger overlap objects to handle grabbing inside overlap volume */
	UPROPERTY()
	UWeArtTouchableObject* MiddleOverlapObj;
	/** Pointer to thumb finger overlap objects to handle grabbing inside overlap volume */
	UPROPERTY()
	UWeArtTouchableObject* ThumbOverlapObj;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetConditionA() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetConditionB() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetConditionC() const;

	/**
	 * @brief Gets response to actor's primitive component
	 * @param Source The actor with primitive component.
	 * @return ECollisionResponse to actor's primitive.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ECollisionResponse GetResponseToHand(AActor* Source) const;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void InitCommonComponents();

	/**
	 * @brief Initializes finger trackers.
	 * @param ThumbTracking tracking object of thumb.
	 * @param IndexTracking tracking object of index.
	 * @param MiddleTracking tracking object of middle.
	 * @param RingTracking tracking object of Ring.
	 * @param PinkyTracking tracking object of pinky.
	 * @param ThumbHaptic haptic effect handler of thumb.
	 * @param IndexHaptic haptic effect handler of index.
	 * @param MiddleHaptic haptic effect handler of middle.
	 * @param RingHaptic haptic effect handler of Ring.
	 * @param PinkyHaptic haptic effect handler of pinky.
	 * @param PalmHaptic
	 */
	UFUNCTION(BlueprintCallable)
	void Init(UWeArtThimbleTrackingObject* ThumbTracking, UWeArtThimbleTrackingObject* IndexTracking,
	          UWeArtThimbleTrackingObject* MiddleTracking, UWeArtThimbleTrackingObject* RingTracking,
	          UWeArtThimbleTrackingObject* PinkyTracking,
	          UWeArtHapticObject* ThumbHaptic, UWeArtHapticObject* IndexHaptic,
	          UWeArtHapticObject* MiddleHaptic, UWeArtHapticObject* RingHaptic,
	          UWeArtHapticObject* PinkyHaptic, UWeArtHapticObject* PalmHaptic);

	/** Checks hand closing state */
	bool CheckHandClosingState();
	void UpdateEffectsAndFingerValues(const EDeviceGeneration DiverType);
	/** Handles finger points */
	void HandleFingerPoints();
	/**
	 * @brief Updates finger state.
	 * @param FingerState state of finger.
	 * @param FingerType
	 * @param Finger's type.
	 */
	void UpdateFingerState(FUFingerState* FingerState, EWeArtFingerType FingerType);
	/** Handles master points - propagates to slave ones*/
	void HandleMasterPoints();
	/** Gets the maximum value in an array */
	static float GetMaxValue(TArray<float> numbers);
	/** Handles slave points */
	void HandleSlavePoints();
	/** Checks grabbable status */
	bool CheckGrabableStatus();

private:
	/** Pointer to hand animation instance */
	UPROPERTY()
	UWeArtHandAnimInstance* AnimInstance;
	/** Pointer to the physical hand mesh component */
	UPROPERTY()
	USkeletalMeshComponent* PhysicalHand;
	/** Pointer to the virtual hand mesh component */
	UPROPERTY()
	USkeletalMeshComponent* VirtualHand;
	/** Pointer to the virtual hand animation instance */
	UPROPERTY()
	UWeArtVirtualHand* VirtualHandAnim;
	/** Pointer to the debug actor */
	UPROPERTY()
	AWeArtDebugActor* DebugActor;
	/** Fingers dictionary */
	FWeArtFingersDictionary FingersDictionary;

	/** Gets the touched object by finger */
	UWeArtTouchableObject* GetTouchedObjectByFinger(const FVector& Location) const;

	const FName PhysicalHandTag = "HandMesh";
	const FName PhantomHandTag = "PhantomHand";
	const FName VirtualHandTag = "VirtualHand";
	/** Finger trace radius */
	const float FingerTraceRadius = 1.4f;
	/** Contact direction threshold */
	const float ContactDirectionThreshold = 0.3f;
	/** Palm box trace extent */
	const FVector PalmBoxTraceExtent = FVector(2.3, 5, 5);
	/** Check bounds point radius */
	const float CheckBoundsPointRadius = 0.15f;
	/** Closure threshold */
	const float ClosureThreshold = 0.4f;
	/** Finger close threshold */
	const float FingerCloseThreshold = 0.06f;
	/** Hand closing state */
	HandClosingState ClosingState = HandClosingState::Open;
	/** Grasping state */
	GraspingState GraspingState = GraspingState::Released;
	/** Initialization flag */
	bool bInitialized = false;
	/** Master contact points count */
	int MasterContactPointsCount = 0;
	/** Slave contact points count */
	int SlaveContactPointsCount = 0;

	/** Grab conditions */
	bool ConditionA;
	bool ConditionB;
	bool ConditionC;
	
	/** Contacted points array */
	TArray<WeArtContactPointPartType> ContactedPoints;
	UPROPERTY(EditAnywhere, Category = "Touch Diver Units | Haptic Objects")
	UWeArtHapticObject* ThumbHapticObject;
	UPROPERTY(EditAnywhere, Category = "Touch Diver Units | Haptic Objects")
	UWeArtHapticObject* IndexHapticObject;
	UPROPERTY(EditAnywhere, Category = "Touch Diver Units | Haptic Objects")
	UWeArtHapticObject* MiddleHapticObject;
	UPROPERTY(EditAnywhere, Category = "Touch Diver Units | Haptic Objects")
	UWeArtHapticObject* RingHapticObject;
	UPROPERTY(EditAnywhere, Category = "Touch Diver Units | Haptic Objects")
	UWeArtHapticObject* PinkyHapticObject;
	UPROPERTY(EditAnywhere, Category = "Touch Diver Units | Haptic Objects")
	UWeArtHapticObject* PalmHapticObject;
	
	bool bFoundGraspEvents;
	UPROPERTY(EditAnywhere, Category = "Touch Diver Units | Tracking Objects")
	UWeArtThimbleTrackingObject* ThumbTrackingObject;
	UPROPERTY(EditAnywhere, Category = "Touch Diver Units | Tracking Objects")
	UWeArtThimbleTrackingObject* IndexTrackingObject;
	UPROPERTY(EditAnywhere, Category = "Touch Diver Units | Tracking Objects")
	UWeArtThimbleTrackingObject* MiddleTrackingObject;
	UPROPERTY(EditAnywhere, Category = "Touch Diver Units | Tracking Objects")
	UWeArtThimbleTrackingObject* RingTrackingObject;
	UPROPERTY(EditAnywhere, Category = "Touch Diver Units | Tracking Objects")
	UWeArtThimbleTrackingObject* PinkyTrackingObject;
public:
	/** Returns the thumb tracking object */
	UFUNCTION(BlueprintCallable)
	UWeArtThimbleTrackingObject* GetThumbTrackingObject() const;
	/** Returns the index tracking object */
	UFUNCTION(BlueprintCallable)
	UWeArtThimbleTrackingObject* GetIndexTrackingObject() const;
	/** Returns the middle tracking object */
	UFUNCTION(BlueprintCallable)
	UWeArtThimbleTrackingObject* GetMiddleTrackingObject() const;
	/** Returns the Annular tracking object */
	UFUNCTION(BlueprintCallable)
	UWeArtThimbleTrackingObject* GetRingTrackingObject() const;
	/** Returns the Pinky tracking object */
	UFUNCTION(BlueprintCallable)
	UWeArtThimbleTrackingObject* GetPinkyTrackingObject() const;

	/** Returns tracking object by finger type */
	FUFingerState GetStateByType(EWeArtFingerType FingerType) const;
	
	/** Traces a ray */
	FHitResult TraceRay(const FVector& StartLoc, const FVector& Direction, bool Debug) const;
	/** Traces a capsule */
	AActor* TraceCapsule(FVector Location, float Radius, FVector& HitLocation, FVector& HitNormal, bool Debug = false) const;
	/** Traces a capsule and returns out actors */
	AActor* TraceCapsuleOutActors(FVector Location, float Radius, FVector& HitLocation, FVector& HitNormal, TArray<AActor*>& OutActors, bool Debug = false) const;
	/** Traces a box */
	AActor* TraceBox(FVector Location, FVector Extent, FRotator Orientation, bool Debug = false) const;
	const USkeletalMeshComponent* GetPhysicalHand() const;

};
