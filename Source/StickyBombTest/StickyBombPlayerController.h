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
	AStickyBombPlayerController();
	
	void SetInteractionWidgetEnabled(bool Enabled);
	//void RemoveInteractionWidget();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> PlayerHudWidget;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> InteractionWidgetClass;

	UUserWidget* InteractionWidget;
};
