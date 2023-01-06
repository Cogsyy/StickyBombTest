// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StickyBombPlayerController.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class STICKYBOMBTEST_API AStickyBombPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UUserWidget* CreateInteractionWidget();

protected:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> InteractionWidgetClass;
};