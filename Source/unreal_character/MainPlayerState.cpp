// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerState.h"
#include "MainSaveGame.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "AttributeComponent.h"

void AMainPlayerState::SavePlayerState_Implementation(UMainSaveGame* SaveObject)
{
	if (SaveObject)
	{
		// Gather all relevant data for player
		FPlayerSaveData SaveData;
		// Stored as FString for simplicity (original Steam ID is uint64)
		SaveData.PlayerID = GetUniqueId().ToString();

		// May not be alive while we save
		if (APawn* MyPawn = GetPawn())
		{
			UAttributeComponent* AttributeComp = UAttributeComponent::GetAttributes(MyPawn);
			if (ensure(AttributeComp))
			{
				SaveData.Health = AttributeComp->GetHealth();
				SaveData.Stamina = AttributeComp->GetStamina();
			}
			SaveData.Location = MyPawn->GetActorLocation();
			SaveData.Rotation = MyPawn->GetActorRotation();
			SaveData.bResumeAtTransform = true;
		}
		
		SaveObject->SavedPlayers.Add(SaveData);
	}
}


void AMainPlayerState::LoadPlayerState_Implementation(UMainSaveGame* SaveObject)
{
	if (SaveObject)
	{
		FPlayerSaveData* FoundData = SaveObject->GetPlayerData(this);
		if (FoundData)
		{
			// if (FoundData->HealthPercentage) {
			// 	MyCharacter->HealthPercentage = FoundData->HealthPercentage;
			// }
			// if (FoundData->StaminaPercentage) {
			// 	MyCharacter->StaminaPercentage = FoundData->StaminaPercentage;
			// }

			//Credits = SaveObject->Credits;
			// Makes sure we trigger credits changed event
			// AddCredits(FoundData->Credits);

			// PersonalRecordTime = FoundData->PersonalRecordTime;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not find SaveGame data for player id '%i'."), GetPlayerId());
		}
	}
}

// void AMainPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

// 	DOREPLIFETIME(AMainPlayerState, Credits);
// }