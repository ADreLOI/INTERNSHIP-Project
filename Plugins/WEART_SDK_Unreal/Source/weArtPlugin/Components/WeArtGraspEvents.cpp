// Fill out your copyright notice in the Description page of Project Settings.


#include "WeArtGraspEvents.h"

// Sets default values
AWeArtGraspEvents::AWeArtGraspEvents()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OnMiddlewareHandControllerGrasp.AddUniqueDynamic(this, &ThisClass::OnHandControllerGrasp);
	OnMiddlewareHandControllerRelease.AddUniqueDynamic(this, &ThisClass::OnHandControllerRelease);

}

// Called when the game starts or when spawned
void AWeArtGraspEvents::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeArtGraspEvents::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

