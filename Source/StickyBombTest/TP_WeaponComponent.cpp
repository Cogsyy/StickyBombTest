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

	//GetNetMode() == NM_ListenServer;
	
	if (GetOwner()->HasAuthority())
	{
		//UE_LOG(LogTemp, Log, TEXT("I am the server. Spawning a projectile"));
		SpawnAndFireProjectile();
		Multicast_PlayFireSoundAndAnimation();
	}
	else
	{
		//We're a client, tell the server that i want to shoot
		//UE_LOG(LogTemp, Log, TEXT("I am a client. Asking the server to invoke SpawnAndFireProjectile"));
		Server_Fire();
	}
}

void UTP_WeaponComponent::Server_Fire_Implementation()
{
	//UE_LOG(LogTemp, Log, TEXT("I am the server. A client has asked me to run SpawnAndFireProjectile. Running the SpawnAndFireProjectile now."));
	SpawnAndFireProjectile();
	Multicast_PlayFireSoundAndAnimation();
}

void UTP_WeaponComponent::SpawnAndFireProjectile() const
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
			ActorSpawnParams.Owner = Instigator;
	
			// Spawn the projectile at the muzzle
			AStickyBombProjectile* Projectile = World->SpawnActor<AStickyBombProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			Projectile->InitializePostSpawn(ViewpointLocation, ViewpointRotation);
		}
	}
}

void UTP_WeaponComponent::Multicast_PlayFireSoundAndAnimation_Implementation()
{
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
		UE_LOG(LogTemp, Log, TEXT("Fire sound"));
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
		UE_LOG(LogTemp, Log, TEXT("Fire anim"));
	}
}

void UTP_WeaponComponent::AttachWeapon(AStickyBombTestCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}
	
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
