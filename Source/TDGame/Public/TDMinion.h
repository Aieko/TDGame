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

public:

	ATDMinion();

protected:



	class ATDPaperCharacter* Player;

	FTimerHandle TimerHandle_ResetCatchTime;

	FTimerHandle TimerHandle_ResetCatchAbility;

	UFUNCTION()
		void ResetCatchTime(APawn* CatchedActor);

	UFUNCTION()
		void ResetCatchAbility();

	bool bCanGrap = true;

	virtual void BeginPlay() override;

	void Tick(float DeltaTime) override;
	
public:

	FORCEINLINE void SetNextPathLocation(FVector PathLocation) { NextPathPoint = PathLocation; }

	bool bNeedToReturn = false;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
