#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeArtCommon.h"
#include "WeArtMessages.h"

#include "WeArtThimbleTrackingObject.generated.h"

/// <summary>
/// Object used to track a single thimble object and its closure value
/// </summary>
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class WEARTPLUGIN_API UWeArtThimbleTrackingObject : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWeArtThimbleTrackingObject();
	virtual ~UWeArtThimbleTrackingObject() override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** @brief Handles the received message. */
	void OnMessageReceived(WeArtMessage *msg);

	float GetClosure() const;
	float GetAbduction() const;

	TEnumAsByte<EHandSide> GetHandSide() const;

protected:

	// Hand/finger state variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeArtThimbleTrackingObject)
		TEnumAsByte<EHandSide> handSide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeArtThimbleTrackingObject)
		TEnumAsByte<EActuationPoint> actuationPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = WeArtThimbleTrackingObject)
		float Closure;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = WeArtThimbleTrackingObject)
		float Abduction;

	UPROPERTY()
	class UWeArtController* WeArtController;
};

