// Fill out your copyright notice in the Description page of Project Settings.


#include "TDGame/Public/InventorySystem/TDInventoryComponent.h"
#include "TDGame/Public/InventorySystem/TDPickUp.h"

// Sets default values for this component's properties
UTDInventoryComponent::UTDInventoryComponent()
{
	Capacity = 12;
}


// Called when the game starts
void UTDInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (auto& Item : DefaultItems)
	{
		AddItem(Item);
	}
	
}




bool UTDInventoryComponent::AddItem(class UTDPickUp* Item)
{
	if (Items.Num() >= Capacity || !Item)
	{
		return false;
	}
	Item->OwningInventory = this;
	Item->World = GetWorld();
	Items.Add(Item);

	OnInventoryUpdated.Broadcast();
	
	return true;
}

bool UTDInventoryComponent::RemoveItem(class UTDPickUp* Item)
{
	Item->OwningInventory = nullptr;
	Item->World = nullptr;
	Items.RemoveSingle(Item);

	OnInventoryUpdated.Broadcast();

	return true;
}