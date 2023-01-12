// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "StickyBombPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoCountChanged, int, NewAmmo);

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

	int GetAmmoCurrent();

	UPROPERTY(BlueprintAssignable)
	FOnAmmoCountChanged OnAmmoCountChanged;

protected:
	UPROPERTY(EditAnywhere)
	int AmmoStarting = 3;
	
	int AmmoCurrent;
};
