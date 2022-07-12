// Copyright Epic Games, Inc. All Rights Reserved.

#include "unreal_characterGameMode.h"
#include "MainHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

Aunreal_characterGameMode::Aunreal_characterGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// use our custom HUD class
	HUDClass = AMainHUD::StaticClass();
}

void Aunreal_characterGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetCurrentState(EGamePlayState::EPlaying);

	MyCharacter = Cast<Aunreal_characterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void Aunreal_characterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GetWorld()->GetMapName();

	if (MyCharacter)
	{
		if (FMath::IsNearlyZero(MyCharacter->GetHealth(), 0.001f))
		{
			SetCurrentState(EGamePlayState::EGameOver);
		}
	}
}

EGamePlayState Aunreal_characterGameMode::GetCurrentState() const
{
	return CurrentState;
}

void Aunreal_characterGameMode::SetCurrentState(EGamePlayState NewState)
{
	CurrentState = NewState;
	HandleNewState(CurrentState);
}

void Aunreal_characterGameMode::HandleNewState(EGamePlayState NewState)
{
	switch (NewState)
	{
		case EGamePlayState::EPlaying:
		{
			// do nothing
		}
		break;
		// Unknown/default state
		case EGamePlayState::EGameOver:
		{
			UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
		}
		break;
		// Unknown/default state
		default:
		case EGamePlayState::EUnknown:
		{
			// do nothing
		}
		break;
	}
}