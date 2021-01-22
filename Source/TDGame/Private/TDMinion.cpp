// Fill out your copyright notice in the Description page of Project Settings.


#include "TDMinion.h"
#include "TDGame/Public/TDPaperCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"

void ATDMinion::ResetCatchTime()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	Player->EnableInput(PlayerController);

}

void ATDMinion::ResetCatchAbility()
{
	bCanGrap = true;
}

void ATDMinion::NotifyActorBeginOverlap(AActor * OtherActor)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	ATDPaperCharacter* PlayerPawn = Cast<ATDPaperCharacter>(OtherActor);
	Player = PlayerPawn;

	if (OtherActor == PlayerPawn && bCanGrap)
	{
		bCanGrap = false;
		Player->DisableInput(PlayerController);
		GetWorldTimerManager().SetTimer(TimerHandle_ResetCatchTime, this, &ATDMinion::ResetCatchTime, 0.1f, false);
		
		GetWorldTimerManager().SetTimer(TimerHandle_ResetCatchAbility, this, &ATDMinion::ResetCatchAbility, 5.0f, false);
	}
}