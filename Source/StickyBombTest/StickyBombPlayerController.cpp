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

void AStickyBombPlayerController::SetInteractionWidgetEnabled(bool Enabled)
{
	if (!InteractionWidget)
	{
		InteractionWidget = CreateWidget(this, InteractionWidgetClass, "InteractionWidget");
		InteractionWidget->AddToViewport();
	}
	
	if (Enabled)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

/*void AStickyBombPlayerController::RemoveInteractionWidget()
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}*/
