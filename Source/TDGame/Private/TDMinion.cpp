// Fill out your copyright notice in the Description page of Project Settings.


#include "TDMinion.h"
#include "TDGame/Public/TDPaperCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "TDGame/Public/TDHealthComponent.h"

ATDMinion::ATDMinion()
{
	HealthComp->DefaultHealth = 2;


}

void ATDMinion::ResetCatchTime()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		Player->EnableInput(PlayerController);
	}
	

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
		if (PlayerController)
		{
			Player->DisableInput(PlayerController);
		}
		GetWorldTimerManager().SetTimer(TimerHandle_ResetCatchTime, this, &ATDMinion::ResetCatchTime, 0.1f, false);
		
		GetWorldTimerManager().SetTimer(TimerHandle_ResetCatchAbility, this, &ATDMinion::ResetCatchAbility, 5.0f, false);
	}
}