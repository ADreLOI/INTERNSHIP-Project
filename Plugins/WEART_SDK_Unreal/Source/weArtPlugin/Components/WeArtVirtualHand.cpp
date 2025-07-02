// Fill out your copyright notice in the Description page of Project Settings.


#include "WeArtVirtualHand.h"

ECollisionResponse GetResponseToHand(USkeletalMeshComponent* PhysicalHand, AActor* Source) 
{
	if (Source && PhysicalHand) 
	{
		auto overlapReponse = Source->FindComponentByClass<UPrimitiveComponent>()->GetCollisionResponseToComponent(PhysicalHand);

		return overlapReponse;
	} 

	return ECR_Ignore;
}

ECollisionResponse UWeArtVirtualHand::GetCollisionChannel(USkeletalMeshComponent* Hand, AActor* Comparer)
{
	return GetResponseToHand(Hand, Comparer);
}
