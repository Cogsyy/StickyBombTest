// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StickyBombPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "AmmoCounterWidget.generated.h"



/**
 * 
 */
UCLASS()
class STICKYBOMBTEST_API UAmmoCounterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnAmmoCountChanged(int NewAmmo);

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* AmmoCounterValueLabel;
};
