// Fill out your copyright notice in the Description page of Project Settings.


#include "FoodItem.h"
#include "InventoryComponent.h"
#include "../AttributeComponent.h"

void UFoodItem::Use(AActor* Actor)
{
    if(Actor)
    {
        UAttributeComponent* AttributeComp = UAttributeComponent::GetAttributes(Actor);
        if (ensure(AttributeComp))
        {
            AttributeComp->ApplyHealthChange(Actor, HealthToHeal);
        }

        if (OwningInventory)
        {
            OwningInventory->RemoveItem(this);
        }
    }
}