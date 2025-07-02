
#include "weArtPlugin/Components/WeArtPhysicHandler.h"

#if ENGINE_MAJOR_VERSION == 4
#include <Px.h>
#include <PxTriangleMesh.h>
#include "IPhysXCooking.h"
#include "PhysXPublicCore.h"
#endif

#include "PhysicsEngine/BodySetup.h"
#include "WeArtFingerSockets.h"
#include "WeArtHandController.h"

#if ENGINE_MAJOR_VERSION == 5
#include "Chaos/TriangleMeshImplicitObject.h"
#endif

#include "WeArtPhysicHandler.h"

#include "DrawDebugHelpers.h"
#include "WeArtDeviceTrackingObject.h"
#include "WeArtPhysicsInteractionComponent.h"
#include "Kismet/KismetMathLibrary.h"

UWeArtPhysicHandler::UWeArtPhysicHandler(): PhysicalHand(nullptr), PhantomHand(nullptr), ConstraintComponent(nullptr),
                                            WeArtController(nullptr), TrackingObject(nullptr),
                                            StartedInteraction(false), HandCollisionType(),
                                            Swing1MotionState(ACM_Locked),
                                            Swing2MotionState(ACM_Locked),
                                            TwistMotionState(ACM_Locked)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeArtPhysicHandler::UpdatePhysics()
{
	
	if (!WeArtController) return;

	const auto bodySetup = PhysicalHand->GetBodyInstance(RootBoneName, true)->GetBodySetup();
	for (int i = 0; i < WeArtFingerSockets::FingersSocketsList.Num(); i++)
	{
		auto bodyPart = bodySetup->AggGeom.SphereElems[i];
		auto newLocation = PhysicalHand->
		                   GetSocketTransform(FName(WeArtFingerSockets::FingersSocketsList[i])).GetRelativeTransform(
			                   PhysicalHand->GetBoneTransform(0)).GetLocation();
		bodyPart.SetTransform(FTransform(newLocation));
		bodySetup->AggGeom.SphereElems[i] = bodyPart;
		
	}
	if (WeArtController->GrabbedObject == nullptr)
	{
		ResetBodies(bodySetup);
	}
}

float UWeArtPhysicHandler::GetDistanceBetweenPhysAndPhantomHands() const
{
	return FVector::Distance(PhysicalHand->GetComponentLocation(), PhantomHand->GetComponentLocation());
}


float UWeArtPhysicHandler::GetDistanceBetweenPhysAndPhantomTips(const FName& TipSocketName, FColor DebugColor, bool DebugDistanceLine) const
{
	if (DebugDistanceLine)
	{
		DrawDebugLine(
			GetWorld(),
			PhysicalHand->GetSocketLocation(TipSocketName),
			PhantomHand->GetSocketLocation(TipSocketName), DebugColor,
			false,
			-1.f,
			0,
			0.5f
		);
	}
	return FVector::Distance(PhysicalHand->GetSocketLocation(TipSocketName), PhantomHand->GetSocketLocation(TipSocketName));
}

// Called when the game starts
void UWeArtPhysicHandler::BeginPlay()
{
	Super::BeginPlay();
	WeArtController = Cast<UWeArtHandController>(
		GetOwner()->FindComponentByClass(UWeArtHandController::StaticClass()));
	PhysicalHand = Cast<USkeletalMeshComponent>(
		GetOwner()->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), PhysicalHandTag)[0]);

	ConstraintComponent = Cast<UPhysicsConstraintComponent>(
		GetOwner()->GetComponentsByTag(UPhysicsConstraintComponent::StaticClass(), ConstraintTag)[0]);
	PhantomHand = Cast<USkeletalMeshComponent>(
		GetOwner()->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), PhantomHandTag)[0]);
	WeArtController->OnGrabbed.AddDynamic(this, &UWeArtPhysicHandler::UpdateGrabbedObjectPhysics);
	TrackingObject = WeArtController->GetOwner()->FindComponentByClass<UWeArtDeviceTrackingObject>();
	
	PhysicalHand->GetBodySetup()->bNeverNeedsCookedCollisionData = false;
	WeArtController->OnReleased.AddDynamic(this, &UWeArtPhysicHandler::OnReleased);

	HandCollisionType = PhysicalHand->GetCollisionEnabled();

	CacheConstraintProperties();
}

TArray<FVector> UWeArtPhysicHandler::GetVertices(UStaticMeshComponent* MeshComponent)
{
	TArray<FVector> ResultVertices;

	if (!MeshComponent || !MeshComponent->GetBodySetup())
	{
		return  ResultVertices;
	}

#if ENGINE_MAJOR_VERSION == 4
	for (PxTriangleMesh* EachTriMesh : MeshComponent->GetBodySetup()->TriMeshes)
	{
		if (!EachTriMesh)
		{
			return ResultVertices;
		}
		//~~~~~~~~~~~~~~~~

		//Number of vertices
		PxU32 VertexCount = EachTriMesh->getNbVertices();

		//Vertex array
		const PxVec3* Vertices = EachTriMesh->getVertices();

		//For each vertex, transform the position to match the component Transform 
		for (PxU32 v = 0; v < VertexCount; v++)
		{
			auto vectorVert = P2UVector(Vertices[v]);

			ResultVertices.Add(vectorVert);
		}
	}
#elif ENGINE_MAJOR_VERSION == 5 and ENGINE_MINOR_VERSION < 4
	for (auto triMesh : MeshComponent->GetBodySetup()->ChaosTriMeshes)
	{
		if (!triMesh)
		{
			return ResultVertices;
		}

		auto vertices = triMesh->Particles().AllX();
		for (auto v = 0; v < vertices.Num(); v++)
		{
			ResultVertices.Add(FVector(vertices[v]));
		}
	}
#elif ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4
	for (auto triMesh : MeshComponent->GetBodySetup()->TriMeshGeometries)
	{
		if (!triMesh)
		{
			return ResultVertices;
		}

		auto vertices = triMesh->Particles().AllX();
		for (auto v = 0; v < vertices.Num(); v++)
		{
			ResultVertices.Add(FVector(vertices[v]));
		}
	}
#endif
	return ResultVertices;
}



// Called every frame
void UWeArtPhysicHandler::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UWeArtPhysicHandler::BreakConstraint(bool SimulatesPhysics = false) const
{
	PhysicalHand->SetSimulatePhysics(SimulatesPhysics);
	PhysicalHand->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (!ConstraintComponent->IsBroken())
		ConstraintComponent->BreakConstraint();
}

void UWeArtPhysicHandler::SetConstraint(bool SimulatesPhysics = true) const
{
	ConstraintComponent->SetConstrainedComponents(PhantomHand, RootBoneName, PhysicalHand, RootBoneName);
	PhysicalHand->SetSimulatePhysics(SimulatesPhysics);
	PhysicalHand->SetCollisionEnabled(HandCollisionType);
}

void UWeArtPhysicHandler::FreeAllAngularLimits(UPrimitiveComponent* MeshComp)
{
	ConstraintComponent->SetConstrainedComponents(PhantomHand, RootBoneName, MeshComp, NAME_None);
	auto ConstraintInstance = ConstraintComponent->ConstraintInstance;
	ConstraintInstance.SetAngularSwing1Motion(ACM_Free);
	ConstraintInstance.SetAngularSwing2Motion(ACM_Free);
	ConstraintInstance.SetAngularTwistMotion(ACM_Free);
}

void UWeArtPhysicHandler::RestoreAngularLimits()
{
	auto ConstraintInstance = ConstraintComponent->ConstraintInstance;
	ConstraintInstance.SetAngularSwing1Motion(Swing1MotionState);
	ConstraintInstance.SetAngularSwing2Motion(Swing2MotionState);
	ConstraintInstance.SetAngularTwistMotion(TwistMotionState);
}

void UWeArtPhysicHandler::CacheConstraintProperties()
{
	auto ConstraintInstance = ConstraintComponent->ConstraintInstance;
	Swing1MotionState = ConstraintInstance.GetAngularSwing1Motion();
	Swing2MotionState = ConstraintInstance.GetAngularSwing2Motion();
	TwistMotionState = ConstraintInstance.GetAngularTwistMotion();

	BaseDrive.Stiffness = ConstraintComponent->ConstraintInstance.ProfileInstance.LinearDrive.XDrive.Stiffness;
 	BaseDrive.Damping = ConstraintComponent->ConstraintInstance.ProfileInstance.LinearDrive.XDrive.Damping;
}

void UWeArtPhysicHandler::RestoreConstraintStrengthValues()
{
	ConstraintComponent->ConstraintInstance.ProfileInstance.LinearDrive.XDrive.Stiffness = BaseDrive.Stiffness;
	ConstraintComponent->ConstraintInstance.ProfileInstance.LinearDrive.XDrive.Damping = BaseDrive.Damping;

	ConstraintComponent->ConstraintInstance.ProfileInstance.LinearDrive.YDrive.Stiffness = BaseDrive.Stiffness;
	ConstraintComponent->ConstraintInstance.ProfileInstance.LinearDrive.YDrive.Damping = BaseDrive.Damping;
	
	ConstraintComponent->ConstraintInstance.ProfileInstance.LinearDrive.ZDrive.Stiffness = BaseDrive.Stiffness;
	ConstraintComponent->ConstraintInstance.ProfileInstance.LinearDrive.ZDrive.Damping = BaseDrive.Damping;
}

void UWeArtPhysicHandler::OnCooked()
{
	RecreatePhysicsState();
	PhysicalHand->TermArticulated();

	PhysicalHand->InitArticulated(GetWorld()->GetPhysicsScene());

	ConstraintComponent->TermComponentConstraint();
	ConstraintComponent->InitComponentConstraint();

	ConstraintComponent->SetConstraintReferencePosition(EConstraintFrame::Frame2, FVector(0, 0, 0));
	ConstraintComponent->
		SetConstraintReferenceOrientation(EConstraintFrame::Frame2, FVector(0, 0, 0), FVector(0, 0, 0));	
}

void UWeArtPhysicHandler::UpdateGrabbedObjectPhysics()
{
	auto bodySetup = PhysicalHand->GetBodyInstance(RootBoneName, true)->GetBodySetup();
	if (WeArtController->GrabbedObject == nullptr)
	{
		ResetBodies(bodySetup);
		return;
	}
	
	TArray<UActorComponent*> outComponents;
	WeArtController->GrabbedObject->GetComponents(UStaticMeshComponent::StaticClass(), outComponents, true);
	
	for (auto component : outComponents)
	{
		if (auto mesh = Cast<UStaticMeshComponent>(component))
		{
			auto transform = mesh->GetComponentTransform();
	
			auto mirrored = GetVertices(mesh);
			auto convexBody = FKConvexElem();
	
			convexBody.VertexData = mirrored;
			bodySetup->AggGeom.ConvexElems.Add(convexBody);
	
			auto relativeTransform = transform.GetRelativeTransform(PhysicalHand->GetBoneTransform(0));
			AddConvexBodies(bodySetup, relativeTransform);
		}
	}
	
	bodySetup->InvalidatePhysicsData();
	bodySetup->CreatePhysicsMeshesAsync(FOnAsyncPhysicsCookFinished::CreateLambda([&]([[maybe_unused]] bool flag)
		{
			OnCooked();
		}));
}

void UWeArtPhysicHandler::OnReleased()
{
	RestoreAngularLimits();
	RestoreConstraintStrengthValues();
	SetConstraint();
	StartedInteraction = false;
}

void UWeArtPhysicHandler::HandlePhysicsInteraction()
{
	if (WeArtController && WeArtController->AnchoredTouchedObject && WeArtController->AnchoredTouchedObject->GetIsInteractionEnabled())
	{
		const auto Interactable = WeArtController->AnchoredTouchedObject;
		if (!Interactable) return;

		const auto Constraint = Interactable->GetOwner()->FindComponentByClass<UPhysicsConstraintComponent>();
		if (!Constraint) return;

		const auto MeshComp = Interactable->GetOwner()->FindComponentByClass<UStaticMeshComponent>();
		if (!MeshComp) return;

		if (!StartedInteraction)
		{
			BreakConstraint();
			FreeAllAngularLimits(MeshComp);
			StartedInteraction = true;
		}
	}
}


void UWeArtPhysicHandler::AddSpheresBodies(UBodySetup* BodySetup, const FTransform& RelativeTransform,
                                           const FKAggregateGeom& AggGeom, int startIndex)
{
	const auto spheresBodies = AggGeom.SphereElems;
	if (spheresBodies.Num() == 0) return;
	for (int i = startIndex; i < spheresBodies.Num(); i++)
	{
		BodySetup->AggGeom.SphereElems[BodySetup->AggGeom.SphereElems.Num() - 1].Radius *= RelativeTransform.
			GetMaximumAxisScale();
		BodySetup->AggGeom.SphereElems[BodySetup->AggGeom.SphereElems.Num() - 1].SetTransform(RelativeTransform);
	}
}

void UWeArtPhysicHandler::ResetBodies(UBodySetup* bodySetup)
{
	if (!bodySetup)
	{
		GEngine->AddOnScreenDebugMessage(static_cast<uint64>(WeArtController->GetUniqueID()), 10, FColor::Red, "No body setup");
		return;
	}
	
	bodySetup->AggGeom.ConvexElems = TArray<FKConvexElem>();
	bodySetup->InvalidatePhysicsData();
	
	// Causes crash. Since we're using convex data for collision attaching, we do not need box elements
	//bodySetup->AggGeom.BoxElems = TArray<FKBoxElem>();
	
	for (int32 i = SpherePrimitiveCount; i < bodySetup->AggGeom.SphereElems.Num(); i++)
	{
		bodySetup->AggGeom.SphereElems.RemoveAt(i);
	}
	for (int32 i = CapsulePrimitiveCount; i < bodySetup->AggGeom.SphylElems.Num(); i++)
	{
		bodySetup->AggGeom.SphylElems.RemoveAt(i);
	}

	if (bodySetup->CurrentCookHelper == nullptr)
	{
		bodySetup->CreatePhysicsMeshesAsync(FOnAsyncPhysicsCookFinished::CreateLambda([&](bool flag)
		{
			OnCooked();
		}));	
	}
}

void UWeArtPhysicHandler::AddBoxBodies(UBodySetup* BodySetup, const FTransform& RelativeTransform, const FKAggregateGeom& AggGeom, int startIndex) const
{
	auto boxBodies = AggGeom.BoxElems;
	if (boxBodies.Num() == 0) return;
	UE_LOG(LogTemp, Warning, TEXT("Relative Transform Rotation: %s"), *RelativeTransform.Rotator().ToString());

	for (int i = 0; i < BodySetup->AggGeom.BoxElems.Num(); i++)
	{
		BodySetup->AggGeom.BoxElems[i].SetTransform(RelativeTransform);
		UE_LOG(LogTemp, Error, TEXT("AggGeom Rotation: %s"), *BodySetup->AggGeom.BoxElems[i].GetTransform().Rotator().ToString());

		BodySetup->AggGeom.BoxElems[i].X *= RelativeTransform.GetScale3D().X;
		BodySetup->AggGeom.BoxElems[i].Y *= RelativeTransform.GetScale3D().Y;
		BodySetup->AggGeom.BoxElems[i].Z *= RelativeTransform.GetScale3D().Z;
	}


}

void UWeArtPhysicHandler::AddCapsulesBodies(UBodySetup* BodySetup, const FTransform& RelativeTransform,
                                            const FKAggregateGeom& AggGeom, int startIndex)
{
	const auto capsuleBodies = AggGeom.SphylElems;
	if (capsuleBodies.Num() == 0) return;

	for (int i = startIndex; i < capsuleBodies.Num(); i++)
	{
		BodySetup->AggGeom.SphylElems[BodySetup->AggGeom.SphylElems.Num() - 1].SetTransform(RelativeTransform);
		BodySetup->AggGeom.SphylElems[BodySetup->AggGeom.SphylElems.Num() - 1].Radius *= RelativeTransform.
			GetMinimumAxisScale();
		BodySetup->AggGeom.SphylElems[BodySetup->AggGeom.SphylElems.Num() - 1].Length *= RelativeTransform.
			GetMaximumAxisScale();
	}
}

void UWeArtPhysicHandler::AddConvexBodies(UBodySetup* BodySetup, const FTransform& RelativeTransform)
{
	if (BodySetup->AggGeom.ConvexElems.Num() == 0) return;

	const int convexId = BodySetup->AggGeom.ConvexElems.Num() - 1;

	BodySetup->AggGeom.ConvexElems[convexId].SetTransform(RelativeTransform);
	BodySetup->AggGeom.ConvexElems[convexId].BakeTransformToVerts();
	BodySetup->AggGeom.ConvexElems[convexId].UpdateElemBox();

	
}
