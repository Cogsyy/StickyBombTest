// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"

#include "IInteractable.h"
#include "StickyBombPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AssertionMacros.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController = Cast<AStickyBombPlayerController>(Controller);
}

void UInteractionComponent::TryFindPawnWithInteractable()
{
	CachedPawnWithIInteractable = nullptr;//Assume null each time
	CachedHitActor = nullptr;
	
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
		//DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, LineColor, false, 2.0f);

		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			if (HitActor->Implements<UIInteractable>() && Hit.Distance <= MaxInteractionDistance)
			{
				SetAbleToInteract(HitActor, Cast<APawn>(MyOwner));
				break;
			}
		}
	}

	if (!CachedHitActor)
	{
		SetInteractionWidgetEnabled(false);
	}
}

void UInteractionComponent::SetAbleToInteract(AActor* HitActor, APawn* HitPawnWithInteractable)
{
	CachedHitActor = HitActor;
	CachedPawnWithIInteractable = HitPawnWithInteractable;

	SetInteractionWidgetEnabled(true);
}

void UInteractionComponent::SetInteractionWidgetEnabled(bool Enabled)
{
	if (Enabled)
	{
		PlayerController->CreateInteractionWidget();
	}
	else
	{
		PlayerController->RemoveInteractionWidget();
	}
}

void UInteractionComponent::TryInteract()
{
	if (CachedPawnWithIInteractable != nullptr && CachedHitActor != nullptr)
	{
		IIInteractable::Execute_Interact(CachedHitActor, CachedPawnWithIInteractable);
	}
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TryFindPawnWithInteractable();
}



