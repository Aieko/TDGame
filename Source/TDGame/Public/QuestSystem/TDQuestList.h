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


		UPROPERTY(EditAnywhere)
		FString QuestName;

		UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<AQuest>> Quests;

		
};
