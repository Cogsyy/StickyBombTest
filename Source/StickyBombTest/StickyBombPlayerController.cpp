// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyBombPlayerController.h"

#include "Blueprint/UserWidget.h"

UUserWidget* AStickyBombPlayerController::CreateInteractionWidget()
{
	return CreateWidget(this, InteractionWidgetClass, "InteractionWidget");
}
