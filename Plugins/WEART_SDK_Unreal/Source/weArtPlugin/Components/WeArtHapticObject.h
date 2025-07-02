#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeArtCommon.h"
#include "WeArtEffect.h"
#include  "WeArtMessages.h"
#include <vector>

// Included cause of a bug in UnrealHeaderTool.
// Those are already included in WeArtEffect.h,
// but need to be included there as well.
#include "WeArtTemperature.h"
#include "WeArtForce.h"
#include "WeArtTexture.h"

#include "WeArtHapticObject.generated.h"

// A haptic object class, representing player-controller objects able to interact
// with objects of the @TouchableObject class.
//
// The communication with the lower layers is triggered by adding and removing effects
// (see @AddEffect() and @RemoveEffect()), which subsequently triggers an @UpdateEffects() event.
//
// Such event then sends forward (to, presumably, the middleware)
// messages representing the processed events
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class WEARTPLUGIN_API UWeArtHapticObject : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWeArtHapticObject();

protected:

	// Hand/finger state variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeArtHapticObject, meta = (Bitmask, BitmaskEnum = EHandSide))
	int32 handSideFlag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeArtHapticObject, meta = (Bitmask, BitmaskEnum = EActuationPoint))
	int32 actuationPointFlag;

	// State variables. Serialized.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeArtHapticObject)
	FWeArtTemperature weArtTemperature;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeArtHapticObject)
	FWeArtForce weArtForce;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeArtHapticObject)
	FWeArtTexture weArtTexture;

	// List of effect active on this object
	// Not serialized.
	UPROPERTY()
	TArray<UWeArtEffect*> ActiveEffects;

public:
	/** Add an effect to the haptic object */
	UFUNCTION(BlueprintCallable)
	void AddEffect(UWeArtEffect* effect);
	/** Remove an effect from the haptic object */
	UFUNCTION(BlueprintCallable)
	void RemoveEffect(UWeArtEffect* effect);
	/** Remove the last effect from the haptic object */
	UFUNCTION(BlueprintCallable)
	void RemoveLastEffect();
	/** Clear all effects from the haptic object */
	UFUNCTION(BlueprintCallable)
	void ClearEffects();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UWeArtEffect*> GetActiveEffects() const;
	void HandleSendingTemperature();
	void HandleSendingForce();
	void HandleSendingTexture();
	void SendStopEffects();
	/** Check if the haptic object contains a specific effect */
	UFUNCTION(BlueprintCallable)
	bool ContainsEffect(UWeArtEffect* effect);
	/** Update all effects on the haptic object */
	UFUNCTION(BlueprintCallable)
	void UpdateEffects();
	/** Send a message from the haptic object */
	void SendMessage(WeArtMessage* msg) const;
	/** Indicates whether the object has been touched */
	UPROPERTY(BlueprintReadWrite)
	bool Touched;

	bool SentStop;
	
	int32 GetActuationPoint() const;
};
