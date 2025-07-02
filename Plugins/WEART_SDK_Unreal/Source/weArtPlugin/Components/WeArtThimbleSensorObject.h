// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeArtCommon.h"
#include "WeArtMessages.h"

#include "WeArtThimbleSensorObject.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEARTPLUGIN_API UWeArtThimbleSensorObject : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeArtThimbleSensorObject();

	void OnMessageReceived(WeArtMessage* msg);

	// Hand/finger state variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeArtThimbleSensorObject)
		TEnumAsByte<EHandSide> handSide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeArtThimbleSensorObject)
		TEnumAsByte<EActuationPoint> actuationPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeArtThimbleSensorObject)
		FVector Accelerometer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeArtThimbleSensorObject)
		FVector Gyroscope;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeArtThimbleSensorObject)
		int TimeOfFlight;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
