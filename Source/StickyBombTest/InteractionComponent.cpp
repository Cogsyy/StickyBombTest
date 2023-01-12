// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"

#include "IInteractable.h"
#include "StickyBombPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AssertionMacros.h"

UInteractionComponent::UInteractionComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	APawn* Owner = Cast<APawn>(GetOwner());
	if (Owner != nullptr && Owner->Controller != nullptr)
	{
		PlayerController = Cast<AStickyBombPlayerController>(Owner->Controller);
	}
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (PlayerController)//don't bother looking for interactables if you don't have a controller. If you don't have one you are probably replicated and wouldn't have a HUD
	{
		FVector ViewpointStart;
		FRotator ViewPointRotation;
		PlayerController->GetPlayerViewPoint(ViewpointStart, ViewPointRotation);
		Server_TryFindActorWithInteractable(ViewpointStart, ViewPointRotation);//Inform the server to call TryFindActorWithInteractable as aa multicast
	}
}

void UInteractionComponent::Server_TryFindActorWithInteractable_Implementation(FVector SweepStart, FRotator ViewPointRotation)
{
	Multicast_TryFindActorWithInteractable(SweepStart, ViewPointRotation);
}

//Summary: Line trace from the actor's viewpoint (See parameters) on both clients, and if an actor is hit and implements IInteractable, cache a reference to the hit actor and instigator to
//show an interaction HUD and for calling OnInteract()
void UInteractionComponent::Multicast_TryFindActorWithInteractable_Implementation(FVector SweepStart, FRotator ViewPointRotation)
{
	CachedInstigatorWithInteractableComp = nullptr;//Assume null each time
	CachedHitActor = nullptr;

	FVector SweepEnd;
	SweepEnd = SweepStart + (ViewPointRotation.Vector() * 1000);
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FCollisionQueryParams CollisionParams;
	AActor* MyOwner = GetOwner();
	CollisionParams.AddIgnoredActor(MyOwner);

	TArray<FHitResult> Hits;
	float Radius = 30.f;
	FCollisionShape Shape;
	Shape.SetSphere(Radius);
	
	GetWorld()->SweepMultiByObjectType(Hits, SweepStart, SweepEnd, FQuat::Identity, ObjectQueryParams, Shape, CollisionParams);
	
	for (FHitResult Hit : Hits)
	{
		//FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
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

//Summary: Cache the hit actor and the instigator and set the "Press e to interact" widget active. The cached actors will be used to call OnInteract on later if the player pressed e
void UInteractionComponent::SetAbleToInteract(AActor* HitActor, APawn* InstigatorPawnWithInteractableComp)
{
	CachedHitActor = HitActor;
	CachedInstigatorWithInteractableComp = InstigatorPawnWithInteractableComp;
	
	SetInteractionWidgetEnabled(true);
}

void UInteractionComponent::SetInteractionWidgetEnabled(bool Enabled)
{
	if (PlayerController)//Don't display UI if you don't have a player controller, you are likely a replicated pawn without a hud
	{
		PlayerController->SetInteractionWidgetEnabled(Enabled);
	}
}

void UInteractionComponent::TryInteract()
{
	if (CachedInstigatorWithInteractableComp != nullptr && CachedHitActor != nullptr)
	{
		if (CachedInstigatorWithInteractableComp->HasAuthority())
		{
			Multicast_Interact(CachedHitActor, CachedInstigatorWithInteractableComp);//Cause each client to call OnInteract() on the actor that implements IInteractable
		}
		else
		{
			Server_Interact(CachedHitActor, CachedInstigatorWithInteractableComp);//Ask the server to call a multicast that causes each client to call OnInteract() on the actor that implements IInteractable
		}
	}
}

void UInteractionComponent::Server_Interact_Implementation(AActor* HitActor, APawn* InstigatorWithInteractableComp)
{
	Multicast_Interact(CachedHitActor, CachedInstigatorWithInteractableComp);
}

void UInteractionComponent::Multicast_Interact_Implementation(AActor* HitActor, APawn* InstigatorWithInteractableComp)
{
	IIInteractable::Execute_Interact(HitActor, InstigatorWithInteractableComp);
}



