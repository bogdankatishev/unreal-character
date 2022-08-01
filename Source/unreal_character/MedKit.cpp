// Fill out your copyright notice in the Description page of Project Settings.


#include "MedKit.h"
#include "AttributeComponent.h"

// Sets default values
AMedKit::AMedKit()
{
	OnActorBeginOverlap.AddDynamic(this, &AMedKit::OnOverlap);
}

void AMedKit::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor)
{
	if ( (OtherActor != nullptr ) && (OtherActor != this) ) 
	{
		UAttributeComponent* AttributeComp = UAttributeComponent::GetAttributes(OtherActor);
		if (ensure(AttributeComp))
		{
			AttributeComp->ApplyHealthChange(OtherActor, 100.0f);
			Destroy();
		}
	}
}
