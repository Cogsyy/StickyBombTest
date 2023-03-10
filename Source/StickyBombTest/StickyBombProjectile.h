// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupableActor.h"
#include "StickyBombTestProjectile.h"
#include "PhysicsEngine/RadialForceComponent.h"
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

	void InitializePostSpawn(FVector ViewpointLocation, FRotator ViewpointRotation);

	void InitializeAsClusterGrenade();

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;
	
	virtual void OnInteract(APawn* InstigatorPawn) override;
	
	bool ShouldFlash(bool IsAttached);

	bool ShouldExplode(bool IsAttached);

	void FlashTick(bool IsAttached);

	bool LineTraceSingleByObjectWhereAiming(FHitResult& OutHit, const FVector ViewpointLocation, const FRotator ViewpointRotation);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void InitiateExplosion();

	void SpawnClusterGrenades(TArray<FVector> RandomDirections);
	
	/*UFUNCTION(Server, Reliable)
	void Server_SpawnClusterGrenades(TArray<FVector> RandomDirections);*/

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Explode();
	
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* ExplosionEffectComp;

	UPROPERTY(EditAnywhere)
	URadialForceComponent* RadialForceComp;

	UPROPERTY(EditDefaultsOnly)
	FName MaterialSpeedParam;

	UPROPERTY(EditDefaultsOnly)
	FName MaterialDeltaTimeParam;
	
	UPROPERTY(EditDefaultsOnly)
	float TimeUntilAttachedExplosion = 4;

	UPROPERTY(EditDefaultsOnly)
	float TimeUntilUnattachedExplosion = 8;
	
	UPROPERTY(EditDefaultsOnly)
	float WarningTimeBeforeExplosionInSeconds = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Cluster grenade")
	float ClusterGrenadeChance = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Cluster grenade")
	int MinimumGrenades = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Cluster grenade")
	int MaximumGrenades = 6;
	
	float fDeltaTime = 0;
	
	float ExplodeTimer = 0;

	bool HitSomething = false;

	bool HitCharacter = false;

	bool AttachedToActor = false;

	bool IsFlashing = false;

	bool IsExploding = false;

	bool IsClusterGrenade = false;
};
