// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_CHARACTER_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	AMainPlayerController();

	/* NEW */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	UUserWidget* PauseMenuInstance;

	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();

	void SetupInputComponent() override;

	/* Called when player controller is ready to begin playing, good moment to initialize things like UI which might be too early in BeginPlay 
		(esp. in multiplayer clients where not all data such as PlayerState may have been received yet) */
	virtual void BeginPlayingState() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();

};
