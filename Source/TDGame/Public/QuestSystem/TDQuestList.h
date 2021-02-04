// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "TDGame/Public/QuestSystem/Quest.h"
#include "TDQuestList.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTDQuestList : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()


		UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
		FString QuestName;

		UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
		TArray<TSubclassOf<AQuest>> Quests;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		bool bIsComplete = false;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		bool bIsActive = false;
};
