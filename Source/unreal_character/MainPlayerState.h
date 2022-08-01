// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"

class AMainPlayerState; // Forward declared to satisfy the delegate macros below
class UMainSaveGame;

/**
 * 
 */
UCLASS()
class UNREAL_CHARACTER_API AMainPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(UMainSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(UMainSaveGame* SaveObject);

};