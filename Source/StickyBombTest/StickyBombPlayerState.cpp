// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyBombPlayerState.h"

void AStickyBombPlayerState::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("BeginPlay - ChangeAmmo"));
	ChangeAmmo(AmmoStarting);
}

void AStickyBombPlayerState::ChangeAmmo(int DeltaChange)
{
	const int MaxAmmo = 9999;
	AmmoCurrent = FMath::Clamp<int>(AmmoCurrent + DeltaChange, 0, MaxAmmo);

	OnAmmoCountChanged.Broadcast(AmmoCurrent);
}

bool AStickyBombPlayerState::HasAmmo()
{
	return AmmoCurrent > 0;
}
