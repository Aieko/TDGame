// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventorySystem/TDPickUp.h"
#include "TDFoodPickUp.generated.h"

/**
 * 
 */
UCLASS()
class TDGAME_API UTDFoodPickUp : public UTDPickUp
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item, meta = (ClampMin = 0))
		int32 HealthToHeal;

protected:

	virtual void Use(class ATDPaperCharacter* Character) override;
	
};
