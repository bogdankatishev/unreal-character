// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PauseMenu.h"

AMainPlayerController::AMainPlayerController()
{
	static ConstructorHelpers::FClassFinder<UPauseMenu> PauseMenuObj(TEXT("/Game/UI/BP_PauseMenu"));
	PauseMenuClass = PauseMenuObj.Class;
}

void AMainPlayerController::TogglePauseMenu()
{
	if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
	{
		PauseMenuInstance->RemoveFromParent();
		PauseMenuInstance = nullptr;

		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
		FSlateApplication::Get().SetUserFocusToGameViewport(0);

		// Single-player only
		if (GetWorld()->IsNetMode(NM_Standalone))
		{
			UGameplayStatics::SetGamePaused(this, false);
		}

		return;
	}

	PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
	if (PauseMenuInstance)
	{
		PauseMenuInstance->AddToViewport(100);

		bShowMouseCursor = true;

		// To lock on the menu when it's visible
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(PauseMenuInstance->GetCachedWidget());
		SetInputMode(InputMode);

		// Single-player only
		if (GetWorld()->IsNetMode(NM_Standalone))
		{
			UGameplayStatics::SetGamePaused(this, true);
		}
	}
}

void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("ESC", IE_Pressed, this, &AMainPlayerController::TogglePauseMenu).bExecuteWhenPaused = true;
}


void AMainPlayerController::BeginPlayingState()
{
	BlueprintBeginPlayingState();
}