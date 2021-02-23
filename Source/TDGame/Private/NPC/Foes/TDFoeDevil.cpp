// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Foes/TDFoeDevil.h"
#include "TDGame/Public/TDHealthComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "NPC/TDProjectile.h"
#include "Components/ArrowComponent.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/KismetMathLibrary.h"
ATDFoeDevil::ATDFoeDevil()
{
	HealthComp->DefaultHealth = 4;

	PawnSensingComp->HearingThreshold = 140.0f;
	PawnSensingComp->LOSHearingThreshold = 156.0f;
	
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	ArrowComp->SetupAttachment(FlipbookComponent);

	RequiredDistanceToTarget = 160;
	DistanceToBreakChasing = 240;
}

void ATDFoeDevil::Attacking()
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ProjectileClass != nullptr)
	{
		GetWorld()->SpawnActor<AActor>(ProjectileClass, ArrowComp->GetComponentLocation(), ArrowComp->GetComponentRotation(), ActorSpawnParams);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectileClass is invalid. Set it up in the blueprint of this foe!"));
	}
	 
	 
	 bCanAttack = true;
}

void ATDFoeDevil::BeginPlay()
{

	Super::BeginPlay();


}

void ATDFoeDevil::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	Super::OnNoiseHeard(NoiseInstigator, Location, Volume);

	
		
	

}

void ATDFoeDevil::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bIsDead)
	if (TargetPawn)
	{
		FRotator ArrowRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetPawn->GetActorLocation());
		ArrowRotation += GetActorRotation();
		ArrowRotation.Yaw += 180.f;

		ArrowComp->SetWorldRotation(ArrowRotation);
		if (bCanAttack)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_ResetAttack, this, &ATDFoeDevil::Attacking, 3.0f, false);
			bCanAttack = false;
		}
		
	}
}