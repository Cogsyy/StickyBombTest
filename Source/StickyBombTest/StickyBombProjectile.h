// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupableActor.h"
#include "StickyBombTestProjectile.h"
#include "StickyExplosive.h"
#include "StickyBombProjectile.generated.h"

/**
 * 
 */
UCLASS()
class STICKYBOMBTEST_API AStickyBombProjectile : public APickupableActor
{
	GENERATED_BODY()

public:
	AStickyBombProjectile();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	virtual void OnInteract(APawn* InstigatorPawn) override;
	
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AStickyExplosive> StickyExplosiveClass;
};
