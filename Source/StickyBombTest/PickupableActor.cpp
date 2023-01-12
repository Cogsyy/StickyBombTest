// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupableActor.h"

// Sets default values
APickupableActor::APickupableActor()
{

}

void APickupableActor::Interact_Implementation(APawn* InstigatorPawn)
{
	IIInteractable::Interact_Implementation(InstigatorPawn);

	OnInteract(InstigatorPawn);
}

void APickupableActor::OnInteract(APawn* InstigatorPawn)
{
	
}



