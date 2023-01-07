// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StickyExplosive.generated.h"

UCLASS()
class STICKYBOMBTEST_API AStickyExplosive : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStickyExplosive();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere)
	float TimeUntilAttachedExplosion = 4;

	UPROPERTY(EditAnywhere)
	float TimeUntilUnattachedExplosion = 8;

	UPROPERTY(EditAnywhere)
	float WarningTimeBeforeExplosionInSeconds = 3;
	
	float ExplodeTimer = 0;

	bool IsAttached = false;
};
