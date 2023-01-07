// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyBombProjectile.h"

#include "StickyBombPlayerState.h"
#include "StickyBombTestCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

//Note: Borrowed some of this code from the default projectile, just moved into my own class here
AStickyBombProjectile::AStickyBombProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AStickyBombProjectile::OnHit);

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(RootComponent);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	ExplosionEffectComp = CreateDefaultSubobject<UParticleSystemComponent>("ExplosionEffectComp");
	ExplosionEffectComp->SetupAttachment(CollisionComp);
}

void AStickyBombProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	ExplosionEffectComp->Deactivate();
	StaticMesh->SetScalarParameterValueOnMaterials(MaterialSpeedParam, 0);//Don't flash
}

void AStickyBombProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HitSomething)//Only evaluate this on first hit
	{
		return;
	}

	HitSomething = true;

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)//Hit a character
	{
		//GetRootComponent()->SetSimu
		
		AttachToActor(Character, FAttachmentTransformRules::KeepRelativeTransform);
		SetActorLocation(Hit.ImpactPoint);
		AttachedToActor = true;
	}
}

void AStickyBombProjectile::OnInteract(APawn* InstigatorPawn)
{
	Super::OnInteract(InstigatorPawn);

	APlayerState* PlayerState = UGameplayStatics::GetPlayerState(this, 0);
	AStickyBombPlayerState* StickyBombPlayerState = Cast<AStickyBombPlayerState>(PlayerState);

	StickyBombPlayerState->ChangeAmmo(1);

	Destroy();
}

bool AStickyBombProjectile::ShouldFlash(bool IsAttached)
{
	float TimeUntilExplosion = IsAttached ? TimeUntilAttachedExplosion : TimeUntilUnattachedExplosion;
	return ExplodeTimer > TimeUntilExplosion - WarningTimeBeforeExplosionInSeconds;
}

bool AStickyBombProjectile::ShouldExplode(bool IsAttached)
{
	float TimeUntilExplosion = IsAttached ? TimeUntilAttachedExplosion : TimeUntilUnattachedExplosion;
	return !IsExploding && ExplodeTimer > TimeUntilExplosion;
}

void AStickyBombProjectile::FlashTick(bool IsAttached)
{
	IsFlashing = true;

	float TimeUntilExplosion = IsAttached ? TimeUntilAttachedExplosion : TimeUntilUnattachedExplosion;
	float PercentOfTimeUntilExplosion = (ExplodeTimer - WarningTimeBeforeExplosionInSeconds) / TimeUntilExplosion;
	if (PercentOfTimeUntilExplosion > 1)
	{
		PercentOfTimeUntilExplosion = 1;
	}
	
	float MaxSpeed = 5;
	float Speed = MaxSpeed * PercentOfTimeUntilExplosion;
	
	StaticMesh->SetScalarParameterValueOnMaterials(MaterialSpeedParam, Speed);
	fDeltaTime += GetWorld()->GetDeltaSeconds();
	StaticMesh->SetScalarParameterValueOnMaterials(MaterialDeltaTimeParam, fDeltaTime);
}

void AStickyBombProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HitSomething)
	{
		ExplodeTimer += GetWorld()->DeltaTimeSeconds;
	}

	if (ShouldFlash(AttachedToActor))
	{
		FlashTick(AttachedToActor);
	}

	if (ShouldExplode(AttachedToActor))
	{
		IsExploding = true;
		ExplosionEffectComp->Activate();
		StaticMesh->SetVisibility(false);
		
		SetLifeSpan(1.0f);
	}
}
