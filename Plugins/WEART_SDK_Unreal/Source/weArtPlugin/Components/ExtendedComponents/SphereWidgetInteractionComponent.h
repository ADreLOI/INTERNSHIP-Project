

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "SphereWidgetInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEARTPLUGIN_API USphereWidgetInteractionComponent : public UWidgetInteractionComponent
{
	GENERATED_BODY()
	
public:	
	
	USphereWidgetInteractionComponent();
	
protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void PerformSphereTrace();

	void SimulateClick(const UWidgetComponent* HitWidgetComponent, const FVector2D& HitLocation, FWeakWidgetPath WidgetPath) const;
};
