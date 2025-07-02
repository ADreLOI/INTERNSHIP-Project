#include "WeArtTouchableObject.h"


#include "WeArtPhysicHandler.h"
#include "WeArtPhysicsInteractionComponent.h"

#include "PhysicsEngine/PhysicsHandleComponent.h"


const FUWeArtImpactInfo FUWeArtImpactInfo::none = FUWeArtImpactInfo();

// Sets default values for this component's properties
UWeArtTouchableObject::UWeArtTouchableObject() : TouchEffect(nullptr),
                                                 InteractableObject(nullptr), IsGraspable(false),
                                                 IsSurfaceExploration(false),
                                                 HandRoot(nullptr),
                                                 GrasperParent(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UWeArtTouchableObject::BeginPlay()
{
	Super::BeginPlay();

	TouchEffect = NewObject<UWeArtTouchEffect>(this);
	InteractableObject = GetOwner()->FindComponentByClass<UWeArtPhysicsInteractionComponent>();
}


// Called every frame
void UWeArtTouchableObject::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// update volume texture
	weArtTexture.volume = VolumeTexture;
	if (TouchEffect) TouchEffect->Init(weArtTemperature, weArtStiffness, weArtTexture);
}

void UWeArtTouchableObject::UpdateTemperature(bool active, float value)
{
	weArtTemperature.active = active;
	weArtTemperature.value = FMath::Clamp(value, 0.0f, 1.0f);
}

void UWeArtTouchableObject::UpdateStiffness(bool active, float value)
{
	weArtStiffness.active = active;
	weArtStiffness.value = FMath::Clamp(value, 0.0f, 1.0f);
}

void UWeArtTouchableObject::UpdateTexture(bool active, TextureType textureType)
{
	weArtTexture.active = active;
	weArtTexture.textureType = textureType;
}

void UWeArtTouchableObject::UpdateTextureVolume(float value)
{
	VolumeTexture = FMath::Clamp(value, 0.0f, 100.0f);
}

bool UWeArtTouchableObject::GetIsSurfaceExplorationEnabled()
{
	return IsSurfaceExploration;
}

bool UWeArtTouchableObject::GetIsGraspable()
{
	return IsGraspable;
}

AActor* UWeArtTouchableObject::GetGrasperActor()
{
	return GrasperActor;
}

void UWeArtTouchableObject::HandlePrimitivesOnGrab()
{
	AActor* OwnerActor = GetOwner();

	if (!IsValid(OwnerActor))
	{
		return;
	}
	
	if (!IsValid(InteractableObject) || !InteractableObject->GetIsInteractionEnabled())
	{
		TArray<UActorComponent*> primitives;

		OwnerActor->GetComponents(UPrimitiveComponent::StaticClass(), primitives, true);

		for (const auto primitive : primitives)
		{
			if (const auto CastedPrimitive = Cast<UPrimitiveComponent>(primitive))
			{
				IsSimulatingPhysics.Add(CastedPrimitive->IsSimulatingPhysics());
				CastedPrimitive->SetSimulatePhysics(false);

				OldStatuses.Add(CastedPrimitive->GetCollisionEnabled());
				CastedPrimitive->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
		}	
	}
}

void UWeArtTouchableObject::HandlePrimitivesOnRelease()
{
	if (!InteractableObject || !InteractableObject->GetIsInteractionEnabled())
	{
		
		TArray<UActorComponent*> primitives;

		GetOwner()->GetComponents(UPrimitiveComponent::StaticClass(), primitives, true);

		int32 i = 0;

		for (auto primitive : primitives)
		{
			auto castedPrimitive = Cast<UPrimitiveComponent>(primitive);

			if (castedPrimitive)
			{
				castedPrimitive->SetSimulatePhysics(IsSimulatingPhysics[i]);
				castedPrimitive->SetCollisionEnabled(OldStatuses[i]);
				i++;
			}
		}

		IsSimulatingPhysics.Empty();
		OldStatuses.Empty();
	}
}

void UWeArtTouchableObject::UpdateIsForcedVelocity(bool value)
{
	weArtTexture.isForcedTextureVelocity = value;
}

float UWeArtTouchableObject::GetTemperatureValue()
{
	return weArtTemperature.value;
}

bool UWeArtTouchableObject::GetTemperatureActive()
{
	return weArtTemperature.active;
}

float UWeArtTouchableObject::GetStiffnessValue()
{
	return weArtStiffness.value;
}

bool UWeArtTouchableObject::GetStiffnessActive()
{
	return weArtStiffness.active;
}

TextureType UWeArtTouchableObject::GetTextureType()
{
	return weArtTexture.textureType;
}

FWeArtTexture UWeArtTouchableObject::GetTexture()
{
	return weArtTexture;
}

float UWeArtTouchableObject::GetTextureVolume()
{
	return weArtTexture.volume;
}

bool UWeArtTouchableObject::GetIsForcedVelocity()
{
	return weArtTexture.isForcedTextureVelocity;
}

void UWeArtTouchableObject::Grab(AActor* grasper)
{
	if (!GetOwner())
	{
		return;
	}

	if (!IsValid(grasper))
	{
		return;
	}
	
	GrasperActor = grasper;

	if (!HandRoot && IsValid(GrasperActor))
	{
		TArray<UActorComponent*> Components = GrasperActor->GetComponentsByTag(USkinnedMeshComponent::StaticClass(), FName("HandMesh"));
		if (Components.Num() > 0)
		{
			HandRoot = Cast<USceneComponent>(Components[0]);
		}
	}

	
	if (!HandRoot)
	{
		GEngine->AddOnScreenDebugMessage((uint64)GetUniqueID(), 5.f, FColor::Red, "No HandRoot on Grab!");
		return;
	}
	
	
	HandlePrimitivesOnGrab();

	
	// When we grab some physics interaction object, that can't be moved
	// like free object. Door, levers can be moved on fixed axis, so
	// we attach hand to target object, and move object instead of hand
	if (IsValid(InteractableObject) && InteractableObject->GetIsInteractionEnabled())
	{
		if (IsValid(HandRoot) && !GrasperParent)
        {
            GrasperParent = HandRoot->GetAttachParent();
        }

		if (auto PrimitiveComponent = GetOwner()->FindComponentByClass<UPrimitiveComponent>())
		{
			if (IsValid(HandRoot) && IsValid(PrimitiveComponent))
			{
				HandRoot->AttachToComponent(PrimitiveComponent, FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}
	else
	{
		if (IsValid(HandRoot))
		{
			GetOwner()->AttachToComponent(HandRoot, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}

UWeArtTouchEffect* UWeArtTouchableObject::GetTouchEffect() const
{
	return TouchEffect;
}

void UWeArtTouchableObject::Release(void)
{
	if (!GetOwner())
	{
		return;
	}
	
	GrasperActor = nullptr;

	if (!HandRoot)
	{
		return;
	}
	
	released = true;

	HandlePrimitivesOnRelease();
	
	if (IsValid(InteractableObject) && InteractableObject->GetIsInteractionEnabled())
	{
		if (IsValid(HandRoot))
		{
			HandRoot->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}

		if (IsValid(HandRoot) && IsValid(GrasperParent))
		{
			HandRoot->AttachToComponent(GrasperParent, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
	else
	{
		if (USceneComponent* RootComponent = GetOwner()->GetRootComponent())
		{
			RootComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}
	}
	
	GrasperParent = nullptr;
	HandRoot = nullptr;
	
	graspingState = GraspingState::Released;
}
