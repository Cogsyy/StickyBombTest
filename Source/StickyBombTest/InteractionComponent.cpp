// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"

#include "IInteractable.h"
#include "Kismet/GameplayStatics.h"

UInteractionComponent::UInteractionComponent()
{
	
}

void UInteractionComponent::Interact()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	
	TArray<FHitResult> Hits;
	float Radius = 30.f;
	FCollisionShape Shape;
	Shape.SetSphere(30.0f);

	FVector SweepStart;
	FVector SweepEnd;
	FRotator ViewPointRotation;
	PlayerController->GetPlayerViewPoint(SweepStart, ViewPointRotation);

	SweepEnd = SweepStart + (ViewPointRotation.Vector() * 1000);
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FCollisionQueryParams CollisionParams;
	AActor* MyOwner = GetOwner();
	CollisionParams.AddIgnoredActor(MyOwner);
	
	
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, SweepStart, SweepEnd, FQuat::Identity, ObjectQueryParams, Shape, CollisionParams);
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	
	for (FHitResult Hit : Hits)
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, LineColor, false, 2.0f);
		
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			if (HitActor->Implements<UIInteractable>())
			{
				APawn* MyPawn = Cast<APawn>(MyOwner);
			
				IIInteractable::Execute_Interact(HitActor, MyPawn);
				break;
			}
		}
	}
}



