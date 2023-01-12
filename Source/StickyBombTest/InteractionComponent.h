// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class AStickyBombPlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STICKYBOMBTEST_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();
	
	void TryInteract();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	float MaxInteractionDistance = 400;//Arbitrary good default amount
	
private:
	void SetAbleToInteract(AActor* HitActor, APawn* InstigatorPawnWithInteractableComp);

	void SetInteractionWidgetEnabled(bool Enabled);

	UFUNCTION(Server, Reliable)
	void Server_TryFindActorWithInteractable(FVector SweepStart, FRotator ViewPointRotation);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TryFindActorWithInteractable(FVector SweepStart, FRotator ViewPointRotation);

	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* HitActor, APawn* InstigatorWithInteractableComp);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Interact(AActor* HitActor, APawn* InstigatorWithInteractableComp);

	AStickyBombPlayerController* PlayerController;
	
	AActor* CachedHitActor;
	APawn* CachedInstigatorWithInteractableComp;
};
