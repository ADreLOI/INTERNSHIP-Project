

#include "SphereWidgetInteractionComponent.h"

#include "DrawDebugHelpers.h"
//#include "WebBrowser.h"
#include "Blueprint/WidgetTree.h"
#include "Components/WidgetComponent.h"


USphereWidgetInteractionComponent::USphereWidgetInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void USphereWidgetInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void USphereWidgetInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PerformSphereTrace();
}

void USphereWidgetInteractionComponent::PerformSphereTrace()
{
	if (GetWorld() == nullptr)
	{
		return;
	}

	FVector TraceStart = GetComponentLocation();
	FVector TraceEnd = TraceStart + (GetForwardVector() * InteractionDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	if (bShowDebug)
	{
		DrawDebugSphere(GetWorld(), TraceStart, InteractionDistance, 12, FColor::Blue, false);
	}

	Async(EAsyncExecution::TaskGraph, [this, TraceStart, TraceEnd, Params]()
	{
		TArray<FHitResult> OutHits;
		if (GetWorld()->SweepMultiByChannel(
		OutHits,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		TraceChannel,
		FCollisionShape::MakeSphere(InteractionDistance), // Adjust the sphere radius as needed
		Params
		))
		{
			for (const FHitResult& HitResult : OutHits)
			{
				if (UWidgetComponent* HitWidgetComponent = Cast<UWidgetComponent>(HitResult.GetComponent()))
				{
					if (HitWidgetComponent->IsVisible())
					{
						CustomHitResult = HitResult; // Set the custom hit result
						return;
					}
				}
			}
		}

		CustomHitResult = FHitResult();
	});
}




