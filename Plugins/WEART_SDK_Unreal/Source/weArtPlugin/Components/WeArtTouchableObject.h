#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeArtCommon.h"
#include "WeArtTouchEffect.h"
#include <set>
#include <map>
#include "PhysicsEngine/PhysicsHandleComponent.h"
// Included cause of a bug in UnrealHeaderTool.
// Those are already included in WeArtEffect.h,
// but need to be included there as well.
#include "WeArtClosure.h"
#include "WeArtTemperature.h"
#include "WeArtForce.h"
#include "WeArtTexture.h"
#include "WeArtHapticObject.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Runtime/Launch/Resources/Version.h"
#include "WeArtTouchableObject.generated.h"

class UWeArtPhysicsInteractionComponent;
// A class representing objects providing nontrivial effect to the player's
// @HapticObject. Interacts with the Haptic Objects by adding effect on begin
// overlap and removing then on overlap end.
// The effects themselves are then handled by the Haptic Object itself.
UCLASS( ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent) )
class WEARTPLUGIN_API UWeArtTouchableObject : public USceneComponent
{
	GENERATED_BODY()

	UPROPERTY()
	UWeArtTouchEffect* TouchEffect;
	
public:
	// Sets default values for this component's properties
	UWeArtTouchableObject();
	/** @brief Gets the touch effect. */
	UFUNCTION(BlueprintCallable, Category = "WEART TouchableObject")
	UWeArtTouchEffect* GetTouchEffect() const;

protected:
	// State variables
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Temperature"))
	FWeArtTemperature weArtTemperature;
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Stiffness"))
	FWeArtForce weArtStiffness;

public:
	/** @brief Checks if force feedback is enabled. */
	bool IsForceFeedback() const;

protected:
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Texture"))
		FWeArtTexture weArtTexture;
	
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Volume Texture", ClampMin = 0.0, ClampMax = 100.0))
		float VolumeTexture = WeArtConstants::defaultVolumeTexture;
	
	// Grasping-related variables.
	UPROPERTY()
	AActor* GrasperActor = nullptr;

	// Reference to interactable component, if its attached
	UPROPERTY()
	UWeArtPhysicsInteractionComponent* InteractableObject;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	/** @brief Indicates if simulating physics is enabled. */
	TArray<bool> IsSimulatingPhysics;
	
	/** @brief Current grasping state. */
	GraspingState graspingState = GraspingState::Released;
	
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Graspable"))
	bool IsGraspable;
	
private:
	/* If true, overrides default surface exploration strategy with more constant behaviour */
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Surface Exploration"))
	bool IsSurfaceExploration;

public:
	bool released = false;
	float CollisionMultiplier = WeArtConstants::defaultCollisionMultiplier;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Overlap handlers. Unity's equivalent of "StayOverlapped" is handled by the tick event.
	// Hit events are not currently handled at all, being more specific than their Unity counterparts
	// and not really suitable for our use.

	/** @brief Updates the temperature. */
	UFUNCTION(BlueprintCallable, Category = "WEART TouchableObject")
		void UpdateTemperature(bool active, float value);
	/** @brief Updates the stiffness. */
	UFUNCTION(BlueprintCallable, Category = "WEART TouchableObject")
		void UpdateStiffness(bool active, float value);
	/** @brief Updates the texture. */
	UFUNCTION(BlueprintCallable, Category = "WEART TouchableObject")
		void UpdateTexture(bool active, TextureType textureType);
	/** @brief Gets the temperature value. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WEART TouchableObject")
		float GetTemperatureValue();
	/** @brief Gets is the temperature active. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WEART TouchableObject")
		bool GetTemperatureActive();
	/** @brief Gets the stiffness value. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WEART TouchableObject")
		float GetStiffnessValue();
	/** @brief Gets is the stiffness active. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WEART TouchableObject")
		bool GetStiffnessActive();
	/** @brief Gets the texture type. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WEART TouchableObject")
		TextureType GetTextureType();
	/** @brief Gets copy of the texture object. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WEART TouchableObject")
		FWeArtTexture GetTexture();
	/** @brief Gets the texture volume. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WEART TouchableObject")
		float GetTextureVolume();
	/** @brief Updates if forced velocity is enabled. */
	UFUNCTION(BlueprintCallable, Category = "WEART TouchableObject")
		void UpdateIsForcedVelocity(bool value);
	/** @brief Checks if forced velocity is enabled. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WEART TouchableObject")
		bool GetIsForcedVelocity();
	/** @brief Updates the texture volume. */
	UFUNCTION(BlueprintCallable, Category = "WEART TouchableObject")
		void UpdateTextureVolume(float value);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WEART TouchableObject")
		bool GetIsSurfaceExplorationEnabled();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WEART TouchableObject")
		bool GetIsGraspable();
	UFUNCTION(BlueprintCallable, Category = "WEART TouchableObject")
		AActor* GetGrasperActor();
	void HandlePrimitivesOnGrab();
	void HandlePrimitivesOnRelease();
	/** @brief Handles the grabbing action. */
	void Grab(AActor* grasper); 
	/** @brief Releases the object. */
	void Release(void);

private:
	/** @brief The old collision status. */
	TArray<ECollisionEnabled::Type> OldStatuses;

	UPROPERTY()
	USceneComponent* HandRoot;
	UPROPERTY()
	USceneComponent* GrasperParent;
};
