
#pragma once

#include "CoreMinimal.h"
#include "WeArtTouchableObject.h"
#include "WeArtCommon.h"
#include "GameFramework/Actor.h"
#include "WeArtGraspEvents.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHandControllerGraspSignature, EHandSide, handSide, UWeArtTouchableObject*, weArtTouchableObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHandControllerReleaseSignature, EHandSide, handSide, UWeArtTouchableObject*, weArtTouchableObject);

/**
 * @class AWeArtGraspEvents
 * @brief Handles events related to grasping actions.
 */
UCLASS()
class WEARTPLUGIN_API AWeArtGraspEvents : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeArtGraspEvents();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Event delegate for hand controller grasp */
	UPROPERTY(BlueprintAssignable)
		FOnHandControllerGraspSignature OnMiddlewareHandControllerGrasp;
	/**
	 * @brief Blueprint event for hand controller grasp.
	 * @param handSide The side of the hand involved in the grasp.
	 * @param weArtTouchableObject The touchable object being grasped.
	 */
	UFUNCTION(BlueprintImplementableEvent)
		void OnHandControllerGrasp(EHandSide handSide, UWeArtTouchableObject* weArtTouchableObject);
	/** Event delegate for hand controller release */
	UPROPERTY(BlueprintAssignable)
		FOnHandControllerReleaseSignature OnMiddlewareHandControllerRelease;
	/**
	 * @brief Blueprint event for hand controller release.
	 * @param handSide The side of the hand involved in the release.
	 * @param weArtTouchableObject The touchable object being released.
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnHandControllerRelease(EHandSide handSide, UWeArtTouchableObject* weArtTouchableObject);

};
