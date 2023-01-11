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
		UE_LOG(LogTemp, Log, TEXT("CreateWidget"));
		UUserWidget* HudWidget = CreateWidget(this, PlayerHudWidget, "PlayerHudWidget");
		HudWidget->AddToViewport();
	}
}

void AStickyBombPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
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
