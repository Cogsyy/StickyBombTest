// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IInteractable.h"
#include "GameFramework/Actor.h"
#include "PickupableActor.generated.h"

UCLASS()
class STICKYBOMBTEST_API APickupableActor : public AActor, public IIInteractable
{
	GENERATED_BODY()
	
public:	
	APickupableActor();

	void Interact_Implementation(APawn* InstigatorPawn);

protected:
	virtual void OnInteract(APawn* InstigatorPawn);

};
