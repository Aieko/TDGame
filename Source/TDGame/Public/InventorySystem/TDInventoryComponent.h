// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TDInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TDGAME_API UTDInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTDInventoryComponent();

public:	
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddItem(class UTDPickUp* Item);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool RemoveItem(class UTDPickUp* Item);

	UPROPERTY(EditDefaultsOnly, Instanced)
		TArray<class UTDPickUp*> DefaultItems;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = Inventory)
		int32 Gold;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		int32 Capacity;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
		TArray<class UTDPickUp*> Items;
};
