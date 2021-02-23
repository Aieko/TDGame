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

void ATDMinion::ResetCatchTime(APawn* CatchedActor)
{
	
 	Player->CanMove = true;

}

void ATDMinion::ResetCatchAbility()
{
	bCanGrap = true;
}

void ATDMinion::BeginPlay()
{

	Super::BeginPlay();
	
	
}

void ATDMinion::NotifyActorBeginOverlap(AActor * OtherActor)
{
	ATDPaperCharacter* PlayerPawn = Cast<ATDPaperCharacter>(OtherActor);
	Player = PlayerPawn;

	if (OtherActor == PlayerPawn && bCanGrap)
	{
		bCanGrap = false;
		
		Player->CanMove = false;

		FTimerDelegate TimerDel;

		TimerDel.BindUFunction(this, FName("ResetCatchTime"), Player);
		
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ResetCatchTime, TimerDel, 0.1f, false);


		GetWorldTimerManager().SetTimer(TimerHandle_ResetCatchAbility, this, &ATDMinion::ResetCatchAbility, 5.0f, false);
	}
}