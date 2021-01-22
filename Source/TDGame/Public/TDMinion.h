// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TDFoe.h"
#include "TDMinion.generated.h"

/**
 * 
 */
UCLASS()
class TDGAME_API ATDMinion : public ATDFoe
{
	GENERATED_BODY()

		
protected:

	class ATDPaperCharacter* Player;

	FTimerHandle TimerHandle_ResetCatchTime;

	FTimerHandle TimerHandle_ResetCatchAbility;

	UFUNCTION()
		void ResetCatchTime();

	UFUNCTION()
		void ResetCatchAbility();

	bool bCanGrap = true;
	
public:
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor);
};
