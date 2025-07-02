
#pragma once

#include "CoreMinimal.h"
#include "WeArtController.h"
#include "WeArtHandController.h"
#include "WeArtHapticObject.h"
#include "Components/ActorComponent.h"
#include "WeArtPhysicHandler.h"
#include "WeArtHapticController.generated.h"

USTRUCT()
struct FSurfaceExplorationUnit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName FingerSocket;
	UPROPERTY(EditAnywhere)
	float CapsuleHeight;
	UPROPERTY(EditAnywhere)
	float CapsuleRadius;
};

/**
 * @class UWeArtHapticController
 * @brief Controls haptic feedback for the controller.
 * 
 * This class manages haptic feedback for the controller, including setting finger forces and updating effects.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEARTPLUGIN_API UWeArtHapticController : public UActorComponent
{
	GENERATED_BODY()

	const int32 SURFACE_EXPLORATION_HITS = 2;

	UPROPERTY(EditAnywhere, Category = "Surface Exploration")
	FSurfaceExplorationUnit IndexPoint;
	UPROPERTY(EditAnywhere, Category = "Surface Exploration")
	FSurfaceExplorationUnit MiddlePoint;
	UPROPERTY(EditAnywhere, Category = "Surface Exploration")
	FSurfaceExplorationUnit AnnularPoint;
	UPROPERTY(EditAnywhere, Category = "Surface Exploration")
	FSurfaceExplorationUnit PinkyPoint;
	UPROPERTY(EditAnywhere, Category = "Surface Exploration")
	FSurfaceExplorationUnit ThumbPoint;
	UPROPERTY(EditAnywhere, Category = "Surface Exploration")
	FSurfaceExplorationUnit PalmPoint;

	
	UPROPERTY(EditAnywhere, Category = "Debugging")
	bool DebugEffects;
	UPROPERTY(EditAnywhere, Category = "Debugging")
	bool DebugSurfaceExploration;
	UPROPERTY(EditAnywhere, Category = "Debugging")
	bool DebugFingerForceDistance;
	UPROPERTY(EditAnywhere, Category = "Debugging")
	float ForceCoefficient = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Debugging")
	FColor SurfaceExplorationDebugColor;
	UPROPERTY(EditAnywhere, Category = "Debugging")
	FColor FingerForceRayColor;
public:
	// Sets default values for this component's properties
	UWeArtHapticController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	/** Sets the force for a specific finger */
	void SetFingerForce(UWeArtHapticObject* HapticObject, UWeArtTouchEffect* TouchEffect, EWeArtFingerType FingerType, const FVector& LastLoc, float
	                    LastTime, UWeArtTouchableObject* ExploredTouchableObject = nullptr);
	void ApplyFingerEffect(UWeArtTouchableObject* ContactObject, const UWeArtTouchableObject* OverlapObject, UWeArtHapticObject* HapticObject, const FVector
	                       & LastPos, float LastTime, EWeArtFingerType FingerType);
	
	UWeArtTouchableObject* DefineFingerState(EWeArtFingerType FingerType, float& CurrentFingerClosure, float& ContactClosure) const;
	void SetForceByFactor(const FName& TipSocketName, EWeArtFingerType FingerType);
	void WriteForceToTouchEffect(UWeArtTouchEffect* TouchEffect) const;
	void LogEffects(const UWeArtTouchEffect* TouchEffect, const UWeArtHapticObject* HapticObject) const;

	bool HitObjectFromSocket(const FName& TraceSocket) const;
	bool IsEnoughHitForExploration() const;
	void DrawSurfaceExploration(const FQuat& SocketRotation, const FVector& SocketLocation, float CapsuleRadius, float CapsuleHeight) const;

	/** Handles new strategy for exploring surface with offsetting touch colliders*/
	void ApplySurfaceExplorationFingerEffect(UWeArtHapticObject* UWeArtHapticObject, const UWeArtTouchableObject* OverlapObj, const FName& TraceSocket, const FVector&
	                                         LastPosition, float LastTime, EWeArtFingerType FingerType, float CapsuleRadius, float CapsuleHeight);
	
	/** Working range of new surface exploration strategy */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SurfaceExplorationLineLength = 1.f;
	/** Pointer to the hand controller */
	UPROPERTY()
	UWeArtHandController* HandController;
	/** Pointer to the physics handler */
	UPROPERTY()
	UWeArtPhysicHandler* PhysicHandler;
	/** Dynamic force based on distance */
	float DynamicForceByDistance = 0;
	/** Last position of the index finger */
	FVector LastIndexPosition;
	/** Last time the index finger position was recorded */
	float LastIndexTime;
	
	/** Updates effects */
	void UpdateEffects();
	/** Applies force velocity of texture */
	void ApplyTextureVelocity(UWeArtTouchEffect* TouchEffect, const UWeArtTouchableObject* TouchableObject, const FVector& LastLoc, float LastTime) const;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	UWeArtController* WeArtControllerInstance;
	FName GetTipSocketByType(EWeArtFingerType FingerType) const;

};
