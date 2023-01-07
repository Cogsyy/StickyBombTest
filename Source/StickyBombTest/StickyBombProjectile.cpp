// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyBombProjectile.h"

#include "StickyBombPlayerState.h"
#include "StickyBombTestCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"

//Note: Borrowed some of this code from the default projectile, just moved into my own class here
AStickyBombProjectile::AStickyBombProjectile()
{
	bReplicates = true;
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

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>("RadialForceComp");
	RadialForceComp->SetupAttachment(CollisionComp);
}

/*void AStickyBombProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(AStickyBombProjectile, bDoorOpen, SharedParams);
}*/

void AStickyBombProjectile::InitializePostSpawn()
{
	return;
	//Draw a ray from the middle of the hud (where the crosshairs are) until it hits what you're aiming at, and use this to modify the projectile's speed/velocity
	FVector ViewpointLocation;
	FRotator ViewpointRotation;
	GetInstigator()->Controller->GetPlayerViewPoint(ViewpointLocation, ViewpointRotation);

	FHitResult Hit;
	bool BlockingHit = LineTraceSingleByObjectWhereAiming(Hit, ViewpointLocation, ViewpointRotation);
	
	if (BlockingHit)
	{
		FVector TossVelocity;
	
		UGameplayStatics::SuggestProjectileVelocity(GetWorld(), TossVelocity, ViewpointLocation, Hit.ImpactPoint, 3000.0f);

		ProjectileMovement->InitialSpeed = 3000;
		TossVelocity.Normalize();
		ProjectileMovement->SetVelocityInLocalSpace(TossVelocity);
	}
	else
	{
		ProjectileMovement->InitialSpeed = 3000;
	}
}

//Encapsulation of line trace code to simplify my own, as it takes several lines for a simple line trace
bool AStickyBombProjectile::LineTraceSingleByObjectWhereAiming(FHitResult& OutHit, const FVector ViewpointLocation, const FRotator ViewpointRotation)
{
	FCollisionObjectQueryParams CollisionObjectQueryParams;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetInstigator());
	CollisionQueryParams.AddIgnoredActor(this);
	
	const float TraceDistance = 9999;
	FVector EndTraceLocation = ViewpointLocation + (ViewpointRotation.Vector() * TraceDistance);
	bool BlockingHit = GetWorld()->LineTraceSingleByObjectType(OutHit, ViewpointLocation, EndTraceLocation, CollisionObjectQueryParams, CollisionQueryParams);

	if (BlockingHit)
	{
		FColor LineColor = FColor::Green;
		DrawDebugLine(GetWorld(), ViewpointLocation, OutHit.ImpactPoint, LineColor, true, 5);
	}

	return BlockingHit;
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
		ProjectileMovement->StopMovementImmediately();
		StaticMesh->SetSimulatePhysics(false);
		
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

		RadialForceComp->FireImpulse();
		
		SetLifeSpan(1.0f);
	}
}
