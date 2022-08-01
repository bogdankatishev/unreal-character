// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUDWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "AttributeComponent.h"
#include "Kismet/GameplayStatics.h"

void UMainHUDWidget::NativeTick(const FGeometry & MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
        UAttributeComponent* AttributeComp = UAttributeComponent::GetAttributes(PlayerPawn);
        if (ensure(AttributeComp))
        {
            HealthBar->SetPercent(AttributeComp->GetHealth() / AttributeComp->GetHealthMax());
            HealthText->SetText(FText::FromString(FString::SanitizeFloat(FMath::RoundHalfFromZero((AttributeComp->GetHealth() / AttributeComp->GetHealthMax()) * 100)) + FString(TEXT("%"))));

            StaminaBar->SetPercent(AttributeComp->GetStamina() / AttributeComp->GetStaminaMax());
            StaminaText->SetText(FText::FromString(FString::SanitizeFloat(FMath::RoundHalfFromZero((AttributeComp->GetStamina() / AttributeComp->GetStaminaMax()) * 100)) + FString(TEXT("%"))));
        }
	}
}