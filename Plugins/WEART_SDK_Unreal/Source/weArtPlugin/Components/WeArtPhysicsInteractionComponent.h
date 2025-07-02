
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeArtPhysicsInteractionComponent.generated.h"

// A class representing interaction with physics constraint objects.
// Require @UWeArtTouchableObject and @UWeArtPhysicsHandler on Haptic Hand
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEARTPLUGIN_API UWeArtPhysicsInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
	
	// If object supports physical interaction
	UPROPERTY(EditAnywhere, Category = "WEART Interraction", Meta = (DisplayName = "Interaction Enabled") )
	bool IsInteractionEnabled;
	
public:	
	UWeArtPhysicsInteractionComponent();

	UFUNCTION(BlueprintCallable, Category = "WEART Interraction")
	bool GetIsInteractionEnabled() const;
	UFUNCTION(BlueprintCallable, Category = "WEART Interraction")
	void SetIsInteractionEnabled(bool Enabled);
		
};
