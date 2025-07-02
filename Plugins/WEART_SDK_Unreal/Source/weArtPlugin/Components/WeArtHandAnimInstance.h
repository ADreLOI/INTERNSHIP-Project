
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeArtHandAnimInstance.generated.h"

/**
 * @class UWeArtHandAnimInstance
 * @brief Animates hand movements for haptic feedback.
 */
UCLASS()
class WEARTPLUGIN_API UWeArtHandAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	/** Updates the animation */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Weart/Animation/Update")
	void UpdateAnimation();
	/** Pointer to the haptic hand actor */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Weart/Animation/")
	AActor* HapticHand;
	/** Pinky finger alpha value */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Weart/Animation/")
	float PinkyFingerAlpha;
	/** Ring finger alpha value */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Weart/Animation/")
	float RingFingerAlpha;
	/** Index finger alpha value */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Weart/Animation/")
	float IndexFingerAlpha;
	/** Middle finger alpha value */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Weart/Animation/")
	float MiddleFingerAlpha;
	/** Thumb finger alpha value */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Weart/Animation/")
	float ThumbFingerAlpha;
	/** Thumb finger abduction alpha value */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Weart/Animation/")
	float ThumbFingerAbductionAlpha;
};
