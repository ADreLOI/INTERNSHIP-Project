// Fill out your copyright notice in the Description page of Project Settings.


#include "WeArtDebugActor.h"



// Sets default values
AWeArtDebugActor::AWeArtDebugActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


// Called when the game starts or when spawned
void AWeArtDebugActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeArtDebugActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

