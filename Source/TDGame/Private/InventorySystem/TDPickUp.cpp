// Fill out your copyright notice in the Description page of Project Settings.


#include "TDGame/Public/InventorySystem/TDPickUp.h"
#include "Engine/Texture2D.h"
#include "TDPaperCharacter.h"

// Sets default values
UTDPickUp::UTDPickUp()
{

	ItemName = FText::FromString("Item");
	UseActionText = FText::FromString("Use");

	PickupTexture = CreateDefaultSubobject<UTexture2D>(TEXT("ItemTexture"));
}

void UTDPickUp::Use(ATDPaperCharacter * Character)
{

}

