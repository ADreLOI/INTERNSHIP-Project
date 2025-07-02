
#pragma once

#include "CoreMinimal.h"
#include "..\Common\WeArtFingerState.h"
#include "WeArtHandController.h"
#include "Components/ActorComponent.h"
#include "WeArtGraspingManager.generated.h"

/**
 * @class UWeArtGraspingManager
 * @brief Manages grasping functionality for hand controllers.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WEARTPLUGIN_API UWeArtGraspingManager : public UActorComponent
{
	GENERATED_BODY()

	float graspDelay = 1.f;

	bool bGrabRightAllowed = true;
	bool bGrabLeftAllowed = true;
	
public:
	// Sets default values for this component's properties
	UWeArtGraspingManager();

	/**
	 * @brief Sets the hand actor and its side.
	 * @param HandActor The actor representing the hand.
	 * @param EHandSide The side of the hand (left or right).
	 */
	UFUNCTION(BlueprintCallable)
	void SetHand(AActor* HandActor, EHandSide EHandSide);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	/** Reference to the right hand controller */
	UWeArtHandController* HandControllerR;
	/** Reference to the left hand controller */
	UWeArtHandController* HandControllerL;

	UWeArtHandController* FindWithGrabbed();
	
	/**
	 * @brief Handles grasping with one hand.
	 * @return True if grasping is successful, false otherwise.
	 */
	bool GrabOneHand() const;
	/**
	 * @brief Handles grasping with two hands.
	 * @return In development.
	 */
	bool GrabTwoHands();
	
	/** Checks the current grasping status */
	void CheckGrasping();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	UWeArtHandController* GetLeftController();

	UFUNCTION(BlueprintCallable)
	UWeArtHandController* GetRightController();
};
