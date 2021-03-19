// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Objective.generated.h"

/**
 * 
 */
UCLASS()
class  TDGAME_API AObjective : public AInfo
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObjective();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "O")
		FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "O")
		FName ObjectiveName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "O")
		int32 TotalProgressNeeded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "O")
		int32 CurrentProgress;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "O")
		bool MustBeCompletedToAdvance;

	

	UFUNCTION(BlueprintCallable, Category = "O")
		void Update(int32 Progress);

	UFUNCTION(BlueprintCallable, Category = "O")
		virtual bool IsComplete() const;

	UFUNCTION(BlueprintCallable, Category = "O")
		virtual float GetProgress() const;

};