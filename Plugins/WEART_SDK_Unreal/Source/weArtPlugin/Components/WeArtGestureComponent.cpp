

#include "WeArtGestureComponent.h"

#include "WeArtHandController.h"
#include "WeArtUtilities.h"

void UWeArtGestureComponent::OnTimerExceed()
{
	if (SavedGesture)
	{
		SavedGesture->StartedTimer = false;

		if (LastGestureName == SavedGesture->GestureName)
		{
			OnGestureUniqueChanged.Broadcast(LastGestureName);
		}

		SavedGesture = nullptr;
	}
}

UWeArtGestureComponent::UWeArtGestureComponent(): HandControllerComponent(nullptr), HandSide(EHandSide::Right),
                                                  SavedGesture(nullptr), DebugGestureClosures(false),
                                                  EnabledTeleportation(false),
                                                  EnabledInformationPanel(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UWeArtGestureComponent::AreClosuresRanged(const FGesture& Gesture, bool IsTdPro) const
{
	bool bIsValid = Gesture.IndexClosures.Num() == 2 && Gesture.MiddleClosures.Num() == 2 && Gesture.ThumbClosures.Num() == 2;

	// If the device is TouchDiverPro, we need to check the Annular and Pinky closures
	if (IsTdPro)
	{
		// Check that Annular and Pinky closures are defined (both should have two elements)
		bIsValid = bIsValid && Gesture.AnnularClosures.Num() == 2 && Gesture.PinkyClosures.Num() == 2;
	}

	if (DebugGestureClosures)
	{
		GEngine->AddOnScreenDebugMessage((uint64)GetUniqueID(), 1.5f, FColor::Yellow, Gesture.GestureName.ToString());
	}
	
	return bIsValid;
}

FName UWeArtGestureComponent::GetLastGestureName() const
{
	return LastGestureName;
}

FName UWeArtGestureComponent::GetSavedGestureName() const
{
	return SavedGesture ? SavedGesture->GestureName : "Null";
}

void UWeArtGestureComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const auto HandControllerActorComponent = GetOwner()->GetComponentByClass(UWeArtHandController::StaticClass()))
	{
		HandControllerComponent = Cast<UWeArtHandController>(HandControllerActorComponent);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Gesture component can't work without WeArtHandController component"));
	}
}

void UWeArtGestureComponent::LogGestures(float IndexClosure, float MiddleClosure, float ThumbClosure, bool IsTouchDiverPro, float AnnularClosure, float PinkyClosure, bool indexInRange, bool middleInRange, bool thumbInRange, bool annularInRange, bool pinkyInRange) const
{
	// Display the closure values on screen for debugging
	FString DebugMessage = FString::Printf(TEXT(
		"Index: %.2f, Middle: %.2f, Thumb: %.2f\n"
		"Annular: %.2f, Pinky: %.2f\n"
		"IsTouchDiverPro: %s\n"
		"Index in Range: %s, Middle in Range: %s, Thumb in Range: %s\n"
		"Annular in Range: %s, Pinky in Range: %s"),
	                                       IndexClosure, MiddleClosure, ThumbClosure,
	                                       AnnularClosure, PinkyClosure,
	                                       IsTouchDiverPro ? TEXT("Yes") : TEXT("No"),
	                                       indexInRange ? TEXT("Yes") : TEXT("No"),
	                                       middleInRange ? TEXT("Yes") : TEXT("No"),
	                                       thumbInRange ? TEXT("Yes") : TEXT("No"),
	                                       annularInRange ? TEXT("Yes") : TEXT("No"),
	                                       pinkyInRange ? TEXT("Yes") : TEXT("No")
	);

	// Display the message at the top of the screen (location (0,0))
	GEngine->AddOnScreenDebugMessage((uint64)GetUniqueID()+1, 1.f, FColor::Yellow, DebugMessage);
}

void UWeArtGestureComponent::HandleGestures()
{
	if (HandControllerComponent->GrabbedObject)
	{
	    OnGestureUniqueChanged.Broadcast(FName());
		return;
	}

	if (HandControllerComponent->GetTouchedObjectFromFingers())
	{
		return;
	}
	
	auto IndexClosure = HandControllerComponent->GetIndexTrackingObject()->GetClosure();
	auto MiddleClosure = HandControllerComponent->GetMiddleTrackingObject()->GetClosure();
	auto ThumbClosure = HandControllerComponent->GetThumbTrackingObject()->GetClosure();

	bool IsTouchDiverPro = GetMutableDefault<UWeArtSettings>()->DeviceGeneration == EDeviceGeneration::TouchDiverPro;
	
	auto AnnularClosure = HandControllerComponent->GetRingTrackingObject()
		? HandControllerComponent->GetRingTrackingObject()->GetClosure()
		: -1.f;

	auto PinkyClosure = HandControllerComponent->GetPinkyTrackingObject()
		? HandControllerComponent->GetPinkyTrackingObject()->GetClosure()
		: -1.f;
	
	for (auto& Gesture : Gestures)
	{
		if (!AreClosuresRanged(Gesture, IsTouchDiverPro))
		{
			return;
		}
		
		auto indexInRange = FMath::IsWithinInclusive(IndexClosure, Gesture.IndexClosures[0], Gesture.IndexClosures[1]);
		auto middleInRange = FMath::IsWithinInclusive(MiddleClosure, Gesture.MiddleClosures[0], Gesture.MiddleClosures[1]);
		auto thumbInRange = FMath::IsWithinInclusive(ThumbClosure, Gesture.ThumbClosures[0], Gesture.ThumbClosures[1]);

		auto annularInRange = !IsTouchDiverPro ? true : FMath::IsWithinInclusive(AnnularClosure, Gesture.AnnularClosures[0], Gesture.AnnularClosures[1]);
		auto pinkyInRange = !IsTouchDiverPro ? true : FMath::IsWithinInclusive(PinkyClosure, Gesture.PinkyClosures[0], Gesture.PinkyClosures[1]);

		if (DebugGestureClosures)
		{
			LogGestures(IndexClosure, MiddleClosure, ThumbClosure, IsTouchDiverPro, AnnularClosure, PinkyClosure, indexInRange,
			            middleInRange, thumbInRange, annularInRange, pinkyInRange);
		}
		
		if (indexInRange && middleInRange && thumbInRange && annularInRange && pinkyInRange)
		{
			if (LastGestureName != Gesture.GestureName)
			{
				if (Gesture.WithDelay && !Gesture.StartedTimer)
				{
					if (SavedGesture) SavedGesture->StartedTimer = false;
					GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UWeArtGestureComponent::OnTimerExceed, Gesture.Delay, false);
					SavedGesture = &Gesture;
					Gesture.StartedTimer = true;
				}
				else if (!Gesture.WithDelay)
				{
					OnGestureUniqueChanged.Broadcast(Gesture.GestureName);
				}
			}
			
			LastGestureName = Gesture.GestureName;
			
			return;
		}
	}
		
}


void UWeArtGestureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HandControllerComponent)
	{
		HandleGestures();
	}
	
}

