// Fill out your copyright notice in the Description page of Project Settings.


#include "WeArtGraspingManager.h"

#include "WeArtHandController.h"


// Sets default values for this component's properties
UWeArtGraspingManager::UWeArtGraspingManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Initialization of hands in GraspingManager
void UWeArtGraspingManager::SetHand(AActor* HandActor, EHandSide EHandSide)
{
	auto Controller = Cast<UWeArtHandController>(HandActor->GetComponentByClass(UWeArtHandController::StaticClass()));
	if(Controller == nullptr) return;
	if(EHandSide == EHandSide::Right)
	{
		HandControllerR = Controller;
		Controller->EHandSide = EHandSide::Right;
		
		return;
	}
	else if (EHandSide == EHandSide::Left)
	{
		HandControllerL = Controller;
		Controller->EHandSide = EHandSide::Left;
	}
}

// Called when the game starts
void UWeArtGraspingManager::BeginPlay()
{
	Super::BeginPlay();
}

UWeArtHandController* UWeArtGraspingManager::FindWithGrabbed()
{
	if (HandControllerL->GrabbedObject)
	{
		return HandControllerL;
	}

	if (HandControllerR->GrabbedObject)
	{
		return HandControllerR;
	}

	return nullptr;
}

// Checks if all conditions are met for grabing the object with one hand.
bool UWeArtGraspingManager::GrabOneHand() const
{
	if (bGrabRightAllowed && HandControllerR)
	{
		HandControllerR->HandleGraspingState();
	}
    
	if (bGrabLeftAllowed && HandControllerL)
	{
		HandControllerL->HandleGraspingState();
	}

	const bool IsLeftControllerValid = HandControllerL && HandControllerL->GrabbedObject;
	const bool IsRightControllerValid = HandControllerR && HandControllerR->GrabbedObject;
    
	return IsLeftControllerValid || IsRightControllerValid;
}


// Checks if all conditions are met for grabing the object with two hands.
bool UWeArtGraspingManager::GrabTwoHands()
{
	if (HandControllerL && HandControllerR)
	{
		const auto grabbedController = FindWithGrabbed();

		const auto ObjectFromLeft = HandControllerL->GetTouchedObjectFromFingers();
		const auto ObjectFromRight = HandControllerR->GetTouchedObjectFromFingers();

		if (ObjectFromLeft != ObjectFromRight) return false;
	
		if (grabbedController && grabbedController == HandControllerR && HandControllerL->CanControllerGrab() && bGrabLeftAllowed)
		{
			FTimerHandle TimerHandle;
			bGrabRightAllowed = false;
			GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, [&](){ bGrabRightAllowed = true; }, graspDelay, false);
			HandControllerL->TouchedObject = grabbedController->GrabbedObject->FindComponentByClass<UWeArtTouchableObject>();
			grabbedController->OnReleasedGrabbedObject();
			HandControllerL->HandleGraspingState();
		
			return true;
		}

		if (grabbedController && grabbedController == HandControllerL &&  HandControllerR->CanControllerGrab() && bGrabRightAllowed)
		{
			FTimerHandle TimerHandle;
			bGrabLeftAllowed = false;
			GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, [&](){ bGrabLeftAllowed = true; }, graspDelay, false);
			HandControllerR->TouchedObject = grabbedController->GrabbedObject->FindComponentByClass<UWeArtTouchableObject>();
			grabbedController->OnReleasedGrabbedObject();
			HandControllerR->HandleGraspingState();
		
			return true;
		}
	}
	
	return false;
}

void UWeArtGraspingManager::CheckGrasping()
{
	if(!GrabTwoHands())
		GrabOneHand();
}

// Called every frame
void UWeArtGraspingManager::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(HandControllerL != nullptr || HandControllerR != nullptr)
	{
		CheckGrasping();
	
	}
}

UWeArtHandController* UWeArtGraspingManager::GetLeftController()
{
	return HandControllerL;
}

UWeArtHandController* UWeArtGraspingManager::GetRightController()
{
	return HandControllerR;
}

