// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyBombProjectile.h"

AStickyBombProjectile::AStickyBombProjectile()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(RootComponent);
}
