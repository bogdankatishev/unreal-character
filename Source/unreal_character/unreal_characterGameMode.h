// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "unreal_characterCharacter.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "unreal_characterGameMode.generated.h"

//enum to store the current state of gameplay
UENUM()
enum class EGamePlayState
{
	EPlaying,
	EGameOver,
	EUnknown
};

UCLASS(minimalapi)
class Aunreal_characterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);

	Aunreal_characterGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	Aunreal_characterCharacter* MyCharacter;
	
	/** Returns the current playing state */
	UFUNCTION(BlueprintPure, Category = "Health")
	EGamePlayState GetCurrentState() const;

	/** Sets a new playing state */
	void SetCurrentState(EGamePlayState NewState);

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

private: 
	/**Keeps track of the current playing state */
	EGamePlayState CurrentState;

	/**Handle any function calls that rely upon changing the playing state of our game */
	void HandleNewState(EGamePlayState NewState);
};



