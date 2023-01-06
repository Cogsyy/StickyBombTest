// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyBombPlayerState.h"

void AStickyBombPlayerState::BeginPlay()
{
	Super::BeginPlay();
	ChangeAmmo(AmmoStarting);
}

void AStickyBombPlayerState::ChangeAmmo(int DeltaChange)
{
	const int MaxAmmo = 9999;
	AmmoCurrent = FMath::Clamp(AmmoCurrent + DeltaChange, 0, MaxAmmo);

	OnAmmoCountChanged.Broadcast(AmmoCurrent);
	UE_LOG(LogTemp, Log, TEXT("WeaponComp: Ammo is now %i"), AmmoCurrent);
}

bool AStickyBombPlayerState::HasAmmo()
{
	return AmmoCurrent > 0;
}
