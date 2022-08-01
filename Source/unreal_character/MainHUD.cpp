// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.h"

AMainHUD::AMainHUD()
{
	static ConstructorHelpers::FClassFinder<UMainHUDWidget> MainHUDWidgetObj(TEXT("/Game/UI/BP_MainHUDWidget"));
	HUDWidgetClass = MainHUDWidgetObj.Class;
}

void AMainHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}
}