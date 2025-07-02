/**
* @file WeArtVirtualHand.h
 * @brief Contains the declaration of the UWeArtVirtualHand class.
 */
#pragma once

#include "CoreMinimal.h"
#include "..\Common\WeArtFingerState.h"
#include "EWeArtFingerType.h"

#include "Animation/AnimInstance.h"
#include "WeArtVirtualHand.generated.h"

/**
 * @class UWeArtVirtualHand
 * @brief An animation instance class for virtual hand animations.
 */
UCLASS()
class WEARTPLUGIN_API UWeArtVirtualHand : public UAnimInstance
{
	GENERATED_BODY()
public:
	/**
	 * @brief Blueprint event to check safe finger closure.
	 * @param Type The type of finger being checked.
	 * @param FingerState The state of the finger.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void CheckSafeFingerClosure(EWeArtFingerType Type,FUFingerState FingerState);
	/** @brief Aborts virtual's hand job */
	UFUNCTION(BlueprintImplementableEvent)
	void AbortVirtualHand();
	/** @brief The closure limit for the index finger. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float IndexClosureLimit;
	/** @brief The closure limit for the middle finger. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MiddleClosureLimit;
	/** @brief The closure limit for the thumb finger. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float ThumbClosureLimit;
	/** @brief The abduction limit for the thumb finger. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float ThumbAbdctionLimit;

	/**
	 * @brief Gets the collision channel for the virtual hand.
	 * @param Hand The skeletal mesh component of the hand.
	 * @param Comparer The actor to compare collision with.
	 * @return The collision response for the hand and comparer.
	 */
	UFUNCTION(BlueprintCallable)
	ECollisionResponse GetCollisionChannel(USkeletalMeshComponent* Hand, AActor* Comparer);
};
