// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyBombPlayerController.h"

#include "Blueprint/UserWidget.h"

void AStickyBombPlayerController::CreateInteractionWidget()
{
	InteractionWidget = CreateWidget(this, InteractionWidgetClass, "InteractionWidget");
	InteractionWidget->AddToViewport();
}

void AStickyBombPlayerController::RemoveInteractionWidget()
{
	if (InteractionWidget)
	{
		InteractionWidget->RemoveFromViewport();
	}
}
