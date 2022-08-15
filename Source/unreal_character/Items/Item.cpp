// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

UItem::UItem()
{
    Weight = 1.f;
    ItemDisplayName = FText::FromString("Item");
    UseActionText = FText::FromString("Use");
}

void UItem::Use(AActor* Actor)
{
    
}