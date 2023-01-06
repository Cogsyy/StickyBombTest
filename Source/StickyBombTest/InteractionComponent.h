// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STICKYBOMBTEST_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();
	
	void TryInteract();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float MaxInteractionDistance = 400;//Arbitrary good default amount
	
private:
	void TryFindPawnWithInteractable();

	void SetAbleToInteract(AActor* HitActor, APawn* HitPawnWithInteractable);

	void SetInteractionWidgetEnabled(bool Enabled);

	APlayerController* PlayerController;
	
	UUserWidget* InteractionWidget;
	
	AActor* CachedHitActor;
	APawn* CachedPawnWithIInteractable;
};
