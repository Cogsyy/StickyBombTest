// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmmoDrop.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class STICKYBOMBTEST_API AAmmoDrop : public AActor
{
	GENERATED_BODY()
	
public:	
	AAmmoDrop();

	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere)
	int AmmoGivenOnPickup = 2;//Arbitrary default
};
