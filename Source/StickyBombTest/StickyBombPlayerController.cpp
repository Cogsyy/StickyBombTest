// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyBombPlayerController.h"

#include "Blueprint/UserWidget.h"

AStickyBombPlayerController::AStickyBombPlayerController()
{

}

void AStickyBombPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (HasAuthority() == false)//Server can't create widget
	{
		CreateHudWidget();
	}
}

void AStickyBombPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		CreateHudWidget();
	}
}

void AStickyBombPlayerController::CreateHudWidget()
{
	UUserWidget* HudWidget = CreateWidget(this, PlayerHudWidget, "PlayerHudWidget");
	HudWidget->AddToViewport();
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
