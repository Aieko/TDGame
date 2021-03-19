// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Foes/TDFoeDevil.h"
#include "TDGame/Public/TDHealthComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "NPC/TDProjectile.h"
#include "Components/ArrowComponent.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"


ATDFoeDevil::ATDFoeDevil()
{
	HealthComp->DefaultHealth = 4;

	PawnSensingComp->HearingThreshold = 140.0f;
	PawnSensingComp->LOSHearingThreshold = 156.0f;

	MinionsZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("MinionsZone"));
	MinionsZoneSphere->SetupAttachment(GetRootComponent());
	MinionsZoneSphere->SetSphereRadius(150.f, true);
	MinionsZoneSphere->SetCanEverAffectNavigation(false);
	MinionsZoneSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MinionsZoneSphere->OnComponentEndOverlap.AddDynamic(this, &ATDFoeDevil::OnMinionsEndOverlap);

	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	ArrowComp->SetupAttachment(FlipbookComponent);

	RequiredDistanceToTarget = 25;
	DistanceToBreakChasing = 200;

	bIsRangeFoe = true;

	MaxMinions = 4;
}
//Projectile Attack
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

void ATDFoeDevil::SpawnMinion()

{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (MinionClass != nullptr)
	{
		ATDMinion* Minion = GetWorld()->SpawnActor<ATDMinion>(MinionClass, ArrowComp->GetComponentLocation() + 15.f, FRotator(0.f,0.f,0.f), ActorSpawnParams);
		MinionHorde.Add(Minion);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MinionClass is invalid. Set it up in the blueprint of this foe!"));
	}
	

	bCanSummon = true;

	MinionsAlive++;
	
}

void ATDFoeDevil::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	Super::OnNoiseHeard(NoiseInstigator, Location, Volume);

	
		
	

}

void ATDFoeDevil::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
		
		if (!bIsDead)
		{
			
			
			for (int32 i = 0; i < MinionsAlive-1; i++)
			{
				if (MinionHorde[i]->GetIsDead())
				{
					MinionHorde.RemoveAt(i);

					MinionsAlive--;
				}

			}
				
			
			if (MinionHorde.Num() < MaxMinions && bCanSummon)
			{
				GetWorldTimerManager().SetTimer(TimerHandle_ResetSummon, this, &ATDFoeDevil::SpawnMinion, 5.0f, false);

				bCanSummon = false;
			}
			//if foe is't dead and target not null then cast a projectile with attack function
			if (TargetPawn)
			{
				//Finding mc and setting our arrowcomp
				FRotator ArrowRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetPawn->GetActorLocation());
				ArrowRotation += GetActorRotation();
				ArrowRotation.Yaw += 180.f;

				//Turn arrow component to face player character
				ArrowComp->SetWorldRotation(ArrowRotation);
				if (bCanAttack)
				{
					//Attacking if can attack
					GetWorldTimerManager().SetTimer(TimerHandle_ResetAttack, this, &ATDFoeDevil::Attacking, 3.0f, false, -1.f);
					bCanAttack = false;
				}

				for (int32 i = 0; i < MinionsAlive; i++)
				{
					MinionHorde[i]->SetNextPathLocation(TargetPawn->GetActorLocation());
				}

			}
			else
			{
					GetWorldTimerManager().ClearTimer(TimerHandle_ResetAttack);
					bCanAttack = true;
					
			}
		}
		//if foe is dead then clear all timers
		else
		{
			if (GetWorldTimerManager().IsTimerActive(TimerHandle_ResetAttack) || GetWorldTimerManager().IsTimerActive(TimerHandle_ResetSummon))
				GetWorldTimerManager().ClearTimer(TimerHandle_ResetAttack);
				GetWorldTimerManager().ClearTimer(TimerHandle_ResetSummon);
		}
	
}

void ATDFoeDevil::OnMinionsEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATDMinion* Minion = Cast<ATDMinion>(OtherActor);

	if (!bIsDead)
	{
		if (OtherActor == Minion)
		{
			for (int32 i = 0; i < MinionsAlive; i++)
			{
				if (MinionHorde[i] == OtherActor)
				{
					MinionHorde[i]->SetNextPathLocation(GetActorLocation());
				}
			}
		}
		

	}

}
