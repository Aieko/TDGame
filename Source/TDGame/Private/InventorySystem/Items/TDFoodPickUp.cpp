// Fill out your copyright notice in the Description page of Project Settings.


#include "TDGame/Public/InventorySystem/Items/TDFoodPickUp.h"
#include "TDPaperCharacter.h"
#include "TDGame/Public/TDHealthComponent.h"
#include "TDGame/Public/InventorySystem/TDInventoryComponent.h"

void UTDFoodPickUp::Use(class ATDPaperCharacter* Character)

{
	if (Character)
	{
		Character->HealthComp->Heal(HealthToHeal);

		if (OwningInventory)
		{
			OwningInventory->RemoveItem(this);
		}
	}
	
	
}