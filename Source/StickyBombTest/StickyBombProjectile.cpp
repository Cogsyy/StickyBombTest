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

void AStickyBombProjectile::InitializePostSpawn(FVector ViewpointLocation, FRotator ViewpointRotation)
{
	FHitResult Hit;
	bool BlockingHit = LineTraceSingleByObjectWhereAiming(Hit, ViewpointLocation, ViewpointRotation);
	
	if (BlockingHit)
	{
		FVector TossVelocity;
	
		bool success = UGameplayStatics::SuggestProjectileVelocity(GetWorld(), TossVelocity, ViewpointLocation, Hit.ImpactPoint, ProjectileMovement->InitialSpeed, false, 10.0f, 0.0f, ESuggestProjVelocityTraceOption::DoNotTrace);

		if (success)
		{
			ProjectileMovement->Velocity = TossVelocity;
		}
	}
	else
	{
		ProjectileMovement->InitialSpeed = 3000;
	}
}

void AStickyBombProjectile::InitializeAsClusterGrenade()
{
	IsClusterGrenade = true;//Prevents cluster grenades from spawning more cluster grenades

	ProjectileMovement->InitialSpeed = 400;
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
		//FColor LineColor = FColor::Green;
		//DrawDebugLine(GetWorld(), ViewpointLocation, OutHit.ImpactPoint, LineColor, true, 5);
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
	HitSomething = true;

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)//Hit a character
	{
		bool CanAttach = !AttachedToActor && !ShouldFlash(AttachedToActor);//If you aren't already attached to an actor, and you haven't started flashing yet

		if (CanAttach)
		{
			ProjectileMovement->StopMovementImmediately();
			StaticMesh->SetSimulatePhysics(false);
		
			AttachToActor(Character, FAttachmentTransformRules::KeepRelativeTransform);
			SetActorLocation(Hit.ImpactPoint);
			AttachedToActor = true;	
		}
	}
}

void AStickyBombProjectile::OnInteract(APawn* InstigatorPawn)//Will be called on all clients in multiplayer
{
	Super::OnInteract(InstigatorPawn);

	APlayerState* PlayerState = InstigatorPawn->GetPlayerState<APlayerState>();
	AStickyBombPlayerState* StickyBombPlayerState = Cast<AStickyBombPlayerState>(PlayerState);

	StickyBombPlayerState->ChangeAmmo(1);
	
	if (HasAuthority())
	{
		Destroy();
	}
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

		if (HasAuthority())
		{
			InitiateExplosion();
		}
	}
}

//Must have authority to call this
void AStickyBombProjectile::InitiateExplosion()
{
	Multicast_Explode();
	
	bool ShouldSpawnClusterGrenades = !IsClusterGrenade && FMath::RandRange(0.0f, 1.0f) <= ClusterGrenadeChance;

	if (ShouldSpawnClusterGrenades)
	{
		TArray<FVector> RandomDirections;
		
		int GrenadeCount = FMath::RandRange(MinimumGrenades, MaximumGrenades);
		for (int i = 0; i < GrenadeCount; i++)
		{
			FVector Direction = FMath::VRand();
			RandomDirections.Add(Direction);
		}

		SpawnClusterGrenades(RandomDirections);
	}
}

void AStickyBombProjectile::SpawnClusterGrenades(TArray<FVector> RandomDirections)
{
	for (int i = 0; i < RandomDirections.Num(); i++)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = Cast<APawn>(GetOwner());
	
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = RandomDirections[i].Rotation();
		AStickyBombProjectile* Projectile = GetWorld()->SpawnActor<AStickyBombProjectile>(GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
		Projectile->InitializeAsClusterGrenade();
	}
}

void AStickyBombProjectile::Multicast_Explode_Implementation()//Mostly cosmetic
{
	ExplosionEffectComp->Activate();
	StaticMesh->SetVisibility(false);

	RadialForceComp->FireImpulse();
		
	SetLifeSpan(1.0f);
}
