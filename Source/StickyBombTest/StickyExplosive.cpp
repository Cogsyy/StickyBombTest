// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyExplosive.h"

// Sets default values
AStickyExplosive::AStickyExplosive()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComp;
}

// Called when the game starts or when spawned
void AStickyExplosive::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStickyExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
