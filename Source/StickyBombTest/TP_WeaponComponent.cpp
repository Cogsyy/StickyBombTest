// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "StickyBombTestCharacter.h"
#include "StickyBombTestProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "StickyBombPlayerState.h"
#include "StickyBombProjectile.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	SetIsReplicatedByDefault(true);
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}

void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	
	AStickyBombPlayerState* StickyBombPlayerState = Character->GetController()->GetPlayerState<AStickyBombPlayerState>();

	if (StickyBombPlayerState->HasAmmo())
	{
		StickyBombPlayerState->ChangeAmmo(-1);
	}
	else
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		CallMulticastFireWithParams();
	}
	else
	{
		Server_Fire();
	}
}

void UTP_WeaponComponent::Server_Fire_Implementation()
{
	CallMulticastFireWithParams();
}

void UTP_WeaponComponent::CallMulticastFireWithParams()
{
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	APawn* Instigator = PlayerController->GetPawn();
	const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

	//For drawing rays later.
	//Draw a ray from the middle of the hud (where the crosshairs are) until it hits what you're aiming at, and use this to modify the projectile's speed/velocity
	FVector ViewpointLocation;
	FRotator ViewpointRotation;
	Instigator->Controller->GetPlayerViewPoint(ViewpointLocation, ViewpointRotation);
	
	Multicast_Fire(SpawnLocation, SpawnRotation, ViewpointLocation, ViewpointRotation, Instigator);
}

void UTP_WeaponComponent::Multicast_Fire_Implementation(FVector SpawnLocation, FRotator SpawnRotation, FVector ViewpointLocation, FRotator ViewpointRotation, APawn* Instigator)
{
	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			ActorSpawnParams.Instigator = Instigator;
	
			// Spawn the projectile at the muzzle
			AStickyBombProjectile* Projectile = World->SpawnActor<AStickyBombProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			Projectile->InitializePostSpawn(ViewpointLocation, ViewpointRotation);
		}
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::AttachWeapon(AStickyBombTestCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}
	
	//APlayerController* MyPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	GetOwner()->SetOwner(TargetCharacter);
	
	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	
	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}
