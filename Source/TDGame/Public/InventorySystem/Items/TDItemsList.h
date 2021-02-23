// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TDGame/Public/InventorySystem/TDPickUp.h"
#include "TDItemsList.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTDItemsList : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()


		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText ItemName;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			TArray<class UTDPickUp*> Item;


};
