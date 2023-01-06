// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoDrop.h"

#include "StickyBombPlayerState.h"
#include "StickyBombTestCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAmmoDrop::AAmmoDrop()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(SphereComp);
}

void AAmmoDrop::BeginPlay()
{
	Super::BeginPlay();
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AAmmoDrop::OnBeginOverlap);

	RootComponent = SphereComp;
}

void AAmmoDrop::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AStickyBombTestCharacter* PlayerCharacter = Cast<AStickyBombTestCharacter>(OtherActor);

	if (PlayerCharacter)
	{
		APlayerState* PlayerState = PlayerCharacter->GetPlayerState();
		AStickyBombPlayerState* StickyBombPlayerState = Cast<AStickyBombPlayerState>(PlayerState);

		StickyBombPlayerState->ChangeAmmo(AmmoGivenOnPickup);

		Destroy();
	}
}
