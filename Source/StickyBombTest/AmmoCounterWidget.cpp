// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoCounterWidget.h"

#include "StickyBombPlayerState.h"

void UAmmoCounterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UAmmoCounterWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	APlayerState* PlayerState = GetOwningPlayer()->GetPlayerState<APlayerState>();
	AStickyBombPlayerState* StickyBombPlayerState = Cast<AStickyBombPlayerState>(PlayerState);
	if (ensure(StickyBombPlayerState))
	{
		StickyBombPlayerState->OnAmmoCountChanged.AddDynamic(this, &UAmmoCounterWidget::OnAmmoCountChanged);

		int AmmoCurrent = StickyBombPlayerState->GetAmmoCurrent();
		AmmoCounterValueLabel->SetText(FText::AsNumber(AmmoCurrent));
	}
}

void UAmmoCounterWidget::OnAmmoCountChanged(int NewAmmo)
{
	AmmoCounterValueLabel->SetText(FText::AsNumber(NewAmmo));
}
