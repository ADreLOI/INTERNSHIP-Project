
#pragma once

#include "CoreMinimal.h"
#include "WeArtDeviceTrackingObject.h"

#include "Components/ActorComponent.h"
#include "WeArtHandController.h"

#include "WeArtPhysicHandler.generated.h"


/**
 * @class UWeArtPhysicHandler
 * @brief Handles the physics interactions between physical and phantom hands.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEARTPLUGIN_API UWeArtPhysicHandler : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeArtPhysicHandler();
	/** Update physics function */
	UFUNCTION(BlueprintCallable)
	void UpdatePhysics();
	/** Get the distance between physical and phantom hands */
	float GetDistanceBetweenPhysAndPhantomHands() const;
	/** Get the distance between physical and phantom hands */
	float GetDistanceBetweenPhysAndPhantomTips(const FName& TipSocketName, FColor DebugColor, bool DebugDistanceLine = false) const;
	/** The physical hand skeletal mesh component */
	UPROPERTY()
	USkeletalMeshComponent* PhysicalHand;
	/** The phantom hand skeletal mesh component */
	UPROPERTY()
	USkeletalMeshComponent* PhantomHand;
	/** The physics constraint component */
	UPROPERTY()
	UPhysicsConstraintComponent* ConstraintComponent;
	/** Handles interaction with interactable physics objects with constraint*/
	UFUNCTION(BlueprintCallable)
	void HandlePhysicsInteraction();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	static TArray<FVector> GetVertices(UStaticMeshComponent* MeshComponent);
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void BreakConstraint(bool SimulatesPhysics) const;
	UFUNCTION(BlueprintCallable)
	void SetConstraint(bool SimulatesPhysics) const;
	UFUNCTION(BlueprintCallable)
	void FreeAllAngularLimits(UPrimitiveComponent* MeshComp);
	UFUNCTION(BlueprintCallable)
	void RestoreAngularLimits();
	UFUNCTION(BlueprintCallable)
	void CacheConstraintProperties();
	UFUNCTION(BlueprintCallable)
	void RestoreConstraintStrengthValues();
private:
	
	/** The WeArt hand controller */
	UPROPERTY()
	UWeArtHandController* WeArtController;
	UPROPERTY()
	UWeArtDeviceTrackingObject* TrackingObject;
	
	/** Function called when physics is cooked */
	void OnCooked();
	/** Update grabbed object physics */
	UFUNCTION()
	void UpdateGrabbedObjectPhysics();
	/** Calls on release grabbed object*/
	UFUNCTION()
	void OnReleased();
	/** Add spheres bodies function */
	static void AddSpheresBodies(UBodySetup* BodySetup, const FTransform& RelativeTransform, const FKAggregateGeom& AggGeom, int startIndex);
	/** Reset bodies function */
	void ResetBodies(UBodySetup* bodySetup);
	/** Add box bodies function */
	void AddBoxBodies(UBodySetup* BodySetup, const FTransform& RelativeTransform, const FKAggregateGeom& AggGeom, int startIndex) const;
	/** Add capsules bodies function */
	static void AddCapsulesBodies(UBodySetup* BodySetup, const FTransform& RelativeTransform, const FKAggregateGeom& AggGeom, int startIndex);
	/** Add convex bodies function */
	static void AddConvexBodies(UBodySetup* BodySetup, const FTransform& RelativeTransform);
		
	/** Physics update timer interval */
	float PhysicsUpdateTimerInterval = 0.1;
	/** Can move flag */
	bool CanMove = true;
	/**Flag for controlling constraint states in physics interaction*/
	bool StartedInteraction;
	/** Sphere primitive count */
	const int SpherePrimitiveCount = 30;
	/** Capsule primitive count */
	const int CapsulePrimitiveCount = 3;
	const FName RootBoneName = "HandRig";
	const FName ConstraintTag = "HandConstraint";
	//const FName ConstraintTag = "HandConstraint";
	const FName PhantomHandTag = "PhantomHand";
	const FName PhysicalHandTag = "HandMesh";

	ECollisionEnabled::Type HandCollisionType;

	EAngularConstraintMotion Swing1MotionState;
	EAngularConstraintMotion Swing2MotionState;
	EAngularConstraintMotion TwistMotionState;

	FConstraintDrive BaseDrive; 
};
