// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyBombPlayerController.h"

#include "Blueprint/UserWidget.h"

AStickyBombPlayerController::AStickyBombPlayerController()
{

}

void AStickyBombPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() == false)//Server can't create widget
	{
		UUserWidget* HudWidget = CreateWidget(this, PlayerHudWidget, "PlayerHudWidget");
		HudWidget->AddToViewport();
	}
}

void AStickyBombPlayerController::CreateInteractionWidget()
{
	if (InteractionWidget == nullptr || InteractionWidget && !InteractionWidget->IsInViewport())
	{
		InteractionWidget = CreateWidget(this, InteractionWidgetClass, "InteractionWidget");
		InteractionWidget->AddToViewport();	
	}
}

void AStickyBombPlayerController::RemoveInteractionWidget()
{
	if (InteractionWidget)
	{
		InteractionWidget->RemoveFromViewport();
	}
}
