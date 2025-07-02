
#include "WeArtPhysicsInteractionComponent.h"

UWeArtPhysicsInteractionComponent::UWeArtPhysicsInteractionComponent(): IsInteractionEnabled(false)
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UWeArtPhysicsInteractionComponent::GetIsInteractionEnabled() const
{
	return IsInteractionEnabled;
}

void UWeArtPhysicsInteractionComponent::SetIsInteractionEnabled(bool Enabled)
{
	IsInteractionEnabled = Enabled;
}

