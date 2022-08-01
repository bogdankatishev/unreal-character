// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_CHARACTER_API UPauseMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* ResumeButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* SaveGameButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* QuitButton;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnResumeClick();

	UFUNCTION()
	void OnSaveGameClick();

	UFUNCTION()
	void OnQuitClick();
};
