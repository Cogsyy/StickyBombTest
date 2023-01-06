// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StickyBombTestProjectile.h"
#include "StickyBombProjectile.generated.h"

/**
 * 
 */
UCLASS()
class STICKYBOMBTEST_API AStickyBombProjectile : public AStickyBombTestProjectile
{
	GENERATED_BODY()

public:
	AStickyBombProjectile();

protected:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* StaticMesh;
};
