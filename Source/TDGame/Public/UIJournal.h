// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDGame/Public/QuestSystem/Quest.h"
#include "UIJournal.generated.h"

/**
 * 
 */
UCLASS()
class TDGAME_API UUIJournal : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Quest")
		TArray<TSubclassOf<AQuest>> Quests;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
		TArray<FString> ActiveQuests;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
		TArray<FString> CompleteQuests;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Q")
		bool bIsComplete = false;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DialogSystem")
		void Show();
};
