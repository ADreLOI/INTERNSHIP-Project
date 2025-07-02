// Fill out your copyright notice in the Description page of Project Settings.


#include "WeArtThimbleSensorObject.h"
#include "WeArtController.h"
// Sets default values for this component's properties
UWeArtThimbleSensorObject::UWeArtThimbleSensorObject()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWeArtThimbleSensorObject::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* gameInstance = GetOuter()->GetWorld()->GetGameInstance();
	if (!gameInstance) {
		return;
	}

	UWeArtController* weArtController = gameInstance->GetSubsystem<UWeArtController>();
	if (!weArtController) {
		return;
	}

	if (!weArtController->GetIsAutoRawDataOn())
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("WeArt: Enable raw data in the project settings"));
	}

	weArtController->thimbleSensorObjects.Add(this);
}

void UWeArtThimbleSensorObject::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UGameInstance* gameInstance = GetOuter()->GetWorld()->GetGameInstance();
	if (!gameInstance) {
		return;
	}

	UWeArtController* weArtController = gameInstance->GetSubsystem<UWeArtController>();
	if (!weArtController) {
		return;
	}

	weArtController->thimbleSensorObjects.Remove(this);
}

// Called every frame
void UWeArtThimbleSensorObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeArtThimbleSensorObject::OnMessageReceived(WeArtMessage* msg)
{
	if (msg->getID() == RawSensorsData::ID) {
		RawSensorsData* sensorMsg = static_cast<RawSensorsData*>(msg);
		
		if (sensorMsg->getHand() != handSide)
			return;

		if (sensorMsg->hasSensor(actuationPoint))
		{
			SensorData data = sensorMsg->getSensor(actuationPoint);
			Accelerometer.X = data.accelerometer.x;
			Accelerometer.Y = data.accelerometer.y;
			Accelerometer.Z = data.accelerometer.z;
			Gyroscope.X = data.gyroscope.x;
			Gyroscope.Y = data.gyroscope.y;
			Gyroscope.Z = data.gyroscope.z;
			TimeOfFlight = data.timeOfFlight.distance;
		}
	};
}

