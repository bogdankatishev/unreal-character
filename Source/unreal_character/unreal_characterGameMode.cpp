// Copyright Epic Games, Inc. All Rights Reserved.

#include "unreal_characterGameMode.h"
#include "MainHUD.h"
#include "MainPlayerController.h"
#include "MainPlayerState.h"
#include "unreal_characterCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGameSubsystem.h"
#include "AttributeComponent.h"

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

	// use our custom Player Controller class
	PlayerControllerClass = AMainPlayerController::StaticClass();

	// use our custom Player State class
	PlayerStateClass = AMainPlayerState::StaticClass();
}

void Aunreal_characterGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// (Save/Load logic moved into new SaveGameSubsystem)
	USaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();

	// Optional slot name (Falls back to slot specified in SaveGameSettings class/INI otherwise)
	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	SG->LoadSaveGame(SelectedSaveSlot);
}

void Aunreal_characterGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetCurrentState(EGamePlayState::EPlaying);

	MyCharacter = Cast<Aunreal_characterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void Aunreal_characterGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Calling Before Super:: so we set variables before 'beginplayingstate' is called in PlayerController (which is where we instantiate UI)
	USaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	SG->HandleStartingNewPlayer(NewPlayer);

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// Now we're ready to override spawn location
	// Alternatively we could override core spawn location to use store locations immediately (skipping the whole 'find player start' logic)
	SG->OverrideSpawnTransform(NewPlayer);
}

void Aunreal_characterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GetWorld()->GetMapName();
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

void Aunreal_characterGameMode::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));

	// Handle Player death
	Aunreal_characterCharacter* Player = Cast<Aunreal_characterCharacter>(VictimActor);
	if (Player)
	{
		// Disabled auto-respawn
// 		FTimerHandle TimerHandle_RespawnDelay;
// 		FTimerDelegate Delegate;
// 		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());
// 
// 		float RespawnDelay = 2.0f;
// 		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);

		AMainPlayerState* PS = Player->GetPlayerState<AMainPlayerState>();

		// Reset health back to 100%
		if (APawn* MyPawn = PS->GetPawn())
		{
			UAttributeComponent* AttributeComp = UAttributeComponent::GetAttributes(MyPawn);
			if (ensure(AttributeComp))
			{
				AttributeComp->SetHealth(AttributeComp->GetHealthMax());
			}
		}
		// if (PS)
		// {
		// 	PS->UpdatePersonalRecord(GetWorld()->TimeSeconds);
		// }

		USaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
		// Immediately auto save on death
		SG->WriteSaveGame();

		SetCurrentState(EGamePlayState::EGameOver);
	}

	// Give Credits for kill
	APawn* KillerPawn = Cast<APawn>(Killer);
	// Don't credit kills of self
	if (KillerPawn && KillerPawn != VictimActor)
	{
		// Only Players will have a 'PlayerState' instance, bots have nullptr
		AMainPlayerState* PS = KillerPawn->GetPlayerState<AMainPlayerState>();
		// if (PS) 
		// {
		// 	PS->AddCredits(CreditsPerKill);
		// }
	}
}