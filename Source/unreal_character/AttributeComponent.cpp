// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent.h"
#include "unreal_characterGameMode.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);

UAttributeComponent::UAttributeComponent()
{
	HealthMax = 1000.0f;
	Health = HealthMax;

	StaminaMax = 1000.0f;
	Stamina = StaminaMax;

	SetIsReplicatedByDefault(true);
}


bool UAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetHealthMax());
}


bool UAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}


bool UAttributeComponent::IsFullHealth() const
{
	return Health == HealthMax;
}

float UAttributeComponent::GetHealth() const
{
	return Health;
}

float UAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

void UAttributeComponent::SetHealth(float Delta)
{
	Health = Delta;
}

bool UAttributeComponent::IsFullStamina() const
{
	return Stamina == StaminaMax;
}


float UAttributeComponent::GetStamina() const
{
	return Stamina;
}

float UAttributeComponent::GetStaminaMax() const
{
	return StaminaMax;
}

bool UAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)
	{
		return false;
	}

	if (Delta < 0.0f)
	{
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

		Delta *= DamageMultiplier;
	}

	float OldHealth = Health;
	float NewHealth = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = NewHealth - OldHealth;

	// Is Server?
	if (GetOwner()->HasAuthority())
	{
		Health = NewHealth;

		if (ActualDelta != 0.0f)
		{
			MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
		}

		// Died
		if (ActualDelta < 0.0f && Health == 0.0f)
		{
			Aunreal_characterGameMode* GM = GetWorld()->GetAuthGameMode<Aunreal_characterGameMode>();
			if (GM)
			{
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}

	return ActualDelta != 0;
}

bool UAttributeComponent::ApplyStaminaChange(AActor* InstigatorActor, float Delta)
{
	float OldStamina = Stamina;
	Stamina = FMath::Clamp(Stamina + Delta, 0.0f, StaminaMax);

	float ActualDelta = Stamina - OldStamina;
	if (ActualDelta != 0.0f)
	{
		MulticastStaminaChanged(InstigatorActor, Stamina, ActualDelta);
	}

	return ActualDelta != 0;
}

UAttributeComponent* UAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return FromActor->FindComponentByClass<UAttributeComponent>();
		//return Cast<UAttributeComponent>(FromActor->GetComponentByClass(UAttributeComponent::StaticClass()));
	}

	return nullptr;
}


bool UAttributeComponent::IsActorAlive(AActor* Actor)
{
	UAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (AttributeComp)
	{
		return AttributeComp->IsAlive();
	}

	return false;
}


void UAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}

void UAttributeComponent::MulticastStaminaChanged_Implementation(AActor* InstigatorActor, float NewStamina, float Delta)
{
	OnStaminaChanged.Broadcast(InstigatorActor, this, NewStamina, Delta);
}

void UAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAttributeComponent, Health);
	DOREPLIFETIME(UAttributeComponent, HealthMax);
	
	DOREPLIFETIME(UAttributeComponent, Stamina);
	DOREPLIFETIME(UAttributeComponent, StaminaMax);
}