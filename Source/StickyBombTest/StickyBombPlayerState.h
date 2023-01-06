// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "StickyBombPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoCountChanged, float, NewAmmo);

/**
 * 
 */
UCLASS()
class STICKYBOMBTEST_API AStickyBombPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void ChangeAmmo(int DeltaChange);

	bool HasAmmo();

	UPROPERTY(BlueprintAssignable)
	FOnAmmoCountChanged OnAmmoCountChanged;

private:
	UPROPERTY(EditAnywhere)
	int AmmoStarting = 3;
	
	int AmmoCurrent;
};
