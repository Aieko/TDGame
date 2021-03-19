// Fill out your copyright notice in the Description page of Project Settings.


#include "TDGame/Public/TDFoe.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "TDGame/Public/TDHealthComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "TDGame/Public/TDBase.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "TDGame/Public/TDHealthComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/SphereComponent.h"
#include "TDGame/TDGame.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TDGame/Public/TDPaperCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TDGame/Public/QuestSystem/Objective.h"

static int32 DebugTDFoePathDrawing = 0;
FAutoConsoleVariableRef CVARDebugTrackerBotDrawing(
	TEXT("COOP.DebugTDFoe"),
	DebugTDFoePathDrawing,
	TEXT("Draw Debug Lines for TDFoe"),
	ECVF_Cheat);
// Sets default values
ATDFoe::ATDFoe()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> FoeIdleAsset;
		FConstructorStatics()
			: FoeIdleAsset(TEXT("PaperFlipbook'/Game/TileSets/Foes/Foe_lv1/Sprites/imp_idle_anim_f.imp_idle_anim_f'"))
		{

		}
	};

	struct FConstructorStatics1
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> FoeWalkAsset;
		FConstructorStatics1()
			: FoeWalkAsset(TEXT("PaperFlipbook'/Game/TileSets/Foes/Foe_lv1/Sprites/imp_run_anim_f.imp_run_anim_f'"))
		{

		}
	};


	static FConstructorStatics ConstructorStatics;
	static FConstructorStatics1 ConstructorStatics1;*/


	

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->OnHearNoise.AddDynamic(this, &ATDFoe::OnNoiseHeard);
	PawnSensingComp->HearingThreshold = 65.0f;
	PawnSensingComp->LOSHearingThreshold = 76.0f;
	PawnSensingComp->SightRadius = 0.0f;
	PawnSensingComp->bSeePawns = false;
	PawnSensingComp->SetPeripheralVisionAngle(0.0f);

	PawnNoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitterComp"));

	

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetSphereRadius(11.0f);
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->SetCanEverAffectNavigation(false);
	CollisionComp->SetSimulatePhysics(true);
	CollisionComp->BodyInstance.bOverrideMass = true;
	CollisionComp->BodyInstance.SetMassOverride(50.f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(COLLISION_BASE, ECR_Overlap);
	CollisionComp->BodyInstance.bLockXRotation = true;
	CollisionComp->BodyInstance.bLockYRotation = true;
	CollisionComp->BodyInstance.bLockZRotation = true;
	CollisionComp->SetRelativeRotation(FRotator(0.000000f, 90.000000f, -90.000000f));
	RootComponent = CollisionComp;
	
	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("SpriteComp"));
	FlipbookComponent->SetRelativeLocation(FVector(-1.0f, 0.0f, 5.0f));
	FlipbookComponent->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlipbookComponent->SetGenerateOverlapEvents(false);
	FlipbookComponent->SetCastShadow(false);
	FlipbookComponent->SetCanEverAffectNavigation(false);
	FlipbookComponent->SetupAttachment(GetRootComponent());

	this->BaseEyeHeight = 2.0f;
	
	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));
	MovementComp->MaxSpeed = 40.0f;

	HealthComp = CreateDefaultSubobject<UTDHealthComponent>(TEXT("HealthComp"));
	HealthComp->DefaultHealth = 10;

	HealthComp->OnHealthChanged.AddDynamic(this, &ATDFoe::HandleTakeDamage);

	RequiredDistanceToTarget = 25;
	DistanceToBreakChasing = 120;

	ImpulseForce = 50.f;
	TargetPawn = nullptr;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ATDFoe::BeginPlay()
{
	Super::BeginPlay();

	NextPathPoint = GetNextPathPoint();
	

	if (Controller != nullptr)
	{
		return;
	}
		else if (AIControllerClass != nullptr)
			{
				FActorSpawnParameters SpawnInfo;
				SpawnInfo.Instigator = GetInstigator();
				SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnInfo.OverrideLevel = GetLevel();
				SpawnInfo.ObjectFlags |= RF_Transient;    // We never want to save AI controllers into a map
				AController* NewController = GetWorld()->SpawnActor<AController>(AIControllerClass, GetActorLocation(), GetActorRotation(), SpawnInfo);
				if (NewController != nullptr)
				{
					// if successful will result in setting this->Controller 
					// as part of possession mechanics
					NewController->Possess(this);
				}
			}
	
	//casting to enemy base on level
	ATDBase* EnemyBase = Cast<ATDBase>(Base);


	//validating enemy base
	if (EnemyBase)
	{
		GLog->Log("Go to Enemy Base.");
	}
	//getting first path point for foe ai


	
}

FVector ATDFoe::GetNextPathPoint()
{
	

	ATDBase* EnemyBase = Cast<ATDBase>(Base);

	float NearestTargetDistance = FLT_MAX;

	// if there TargetPawn then chase it
	if (TargetPawn)
	{

		NextPathPoint = TargetPawn->GetActorLocation();

		float Distance = (TargetPawn->GetActorLocation() - GetActorLocation()).Size();

		//if distance to target more than distancetobreakchasing
		if (Distance >= DistanceToBreakChasing)
		{
			TargetPawn = nullptr; //clear reference
			bIsSeenPawn = false;

			//trying to find base, if not than just standing
			//code below was build for the tower defense game, it's actrually all deprecated now
			/*
			if (EnemyBase)
			{
				NextPathPoint = GetNextPathPoint();
			}
			else
			{
				return GetActorLocation();
			}*/
		}

		//checking to face right direction to the target
		if (GetActorLocation().Y < NextPathPoint.Y)
		{
			bIsTurned = false;
		}
		else
		{
			bIsTurned = true;
		}
		return NextPathPoint;

	}

	//failed to find path
	return GetActorLocation();


	// if there EnemyBase and we don't have any pawn around then the target is enemy base
	//code below was build for the tower defense game, it's actrually all deprecated now
	/*
	if (EnemyBase && !bIsSeenPawn)
	{
		float Distance = (EnemyBase->GetActorLocation() - GetActorLocation()).Size();
		if (Distance < NearestTargetDistance)
		{

			NearestTargetDistance = Distance;

		}
	}
	// if EnemyBase is Valid then trying to find path to it
	if (EnemyBase)
	{
		NextPathPoint = EnemyBase->GetActorLocation();

		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, GetActorLocation(), NextPathPoint);

		GetWorldTimerManager().ClearTimer(TimerHandle_RefreshPath);
		GetWorldTimerManager().SetTimer(TimerHandle_RefreshPath, this, &ATDFoe::RefreshPath, 5.0f, false);

		//ArrayOfPaths, should I clear them everytime?
		if (NavPath && NavPath->PathPoints.Num() > 1)
		{
			if (GetActorLocation().Y < NextPathPoint.Y)
			{
				bIsTurned = false;
			}
			else
			{
				bIsTurned = true;
			}
			return NavPath->PathPoints[1];
		}

	}

	*/

	
}

// Called every frame
void ATDFoe::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	//if foe is standing and not stuck
	if (NextPathPoint == GetActorLocation() && !bStuck)
	{
		UpdateFoe();
		//checking when boolean bIsChilling become false
		if (!bIsChilling)
		{
			//pushing the function for walking around
			WalkingAround();
		}
				
	}
	else //if nextpathpoint different to foe location
	{
		UpdateFoe();
		//Calculating distance between foe and next target
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

			//checking distance between foe and his target
			if (DistanceToTarget <= RequiredDistanceToTarget)
			{

				if (!bIsChilling)
				{
					//if we close to target then get next path point
					NextPathPoint = GetNextPathPoint();

					ResetChilling();
					
				}
				else
				{
					NextPathPoint = GetActorLocation();
				}

				//if foe is stucked then setting it in the stucking mode
				if (LastFoeLocation == GetActorLocation())
				{
					bStuck = true;
					DetermineFoeState();
					LastFoeLocation = this->GetActorLocation();
					OriginalRotation = GetActorRotation();
					return;
				}
			}
			//if we don't close to target
			else
			{
				if (CurrentState == EFoeState::Attacking)
				{
					if (bIsRangeFoe)
					{
						if (DistanceToTarget <= DistanceToStopAndShoot)
						{
							GetController()->StopMovement();
						}
						else
						{
							UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), NextPathPoint);
						}
					}
					else
					{
						UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), NextPathPoint);
					}
				}
				else
				{
					UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), NextPathPoint);
				}
		
				if (GetActorLocation().Y <= NextPathPoint.Y)
				{
					bIsTurned = false;
				}
				else
				{
					bIsTurned = true;
				}

			}
			
		//saving our last location coordinates to track down stucking
		LastFoeLocation = this->GetActorLocation();
		OriginalRotation = GetActorRotation();
	}
		
}



void ATDFoe::RefreshPath()
{
	NextPathPoint = GetNextPathPoint();
}

void ATDFoe::ResetChilling()
{
	bIsChilling = !bIsChilling;
	
	GetWorldTimerManager().SetTimer(TimerHandle_FoeChilling, this, &ATDFoe::ResetChilling, rand() % 20 + 5, false);

}

void ATDFoe::UpdateAnimation()
{
	if (!bIsDead)
	{
		const FVector PlayerVelocity = GetVelocity();
		const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

		// Are we moving or standing still?
		UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? FoeWalkAnimation : FoeIdleAnimation;
		if (FlipbookComponent->GetFlipbook() != DesiredAnimation)
		{
			FlipbookComponent->SetFlipbook(DesiredAnimation);
		}

		//Setting Direction of sprite
		if (bIsTurned)
		{
			FlipbookComponent->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
		}

		else
		{
			FlipbookComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		}

	}

	else
	{
		FlipbookComponent->SetFlipbook(DeathAnimation);
	}
			
}

void ATDFoe::UpdateFoe()
{
	UpdateAnimation();
}


void ATDFoe::HandleTakeDamage(UTDHealthComponent * OwningHealthComp, int32 Health, int32 HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	//Pushing while handling damage
	FVector ImpulseDirection = CollisionComp->GetForwardVector();
	if (!bIsTurned)
	{
		ImpulseForce = -ImpulseForce;
	}
	
	CollisionComp->AddImpulse(ImpulseDirection * CollisionComp->GetMass() * ImpulseForce);

	if (ImpulseForce < 0)
	{
		ImpulseForce = -ImpulseForce;
	}
	
	//Visual part of handle damage
	if (MatInst == nullptr)
	{
		MatInst = FlipbookComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, FlipbookComponent->GetMaterial(0));
	}
	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
	
	//cheking if dead or not and clearing all data
	if (Health <= 0)
	{
		bIsDead = true;
		if (GetController() && DeathAnimation)
		{
			GetController()->StopMovement();
			SetLifeSpan(DeathAnimation->GetTotalDuration());
			if (AttachedObjective)
			{
				AttachedObjective->Update(1);
			}
			
			
		}

		TargetPawn = nullptr;
		NextPathPoint = GetActorLocation();
		PawnSensingComp->bHearNoises = false;
		PrimaryActorTick.bCanEverTick = false;
		
	}
}

//Determining states
void ATDFoe::DetermineFoeState()
{
	EFoeState NewState = EFoeState::Default;

	

	if (bStuck && !bIsSeenPawn && !bIsChilling)
	{
		NewState = EFoeState::Stucking;
	}
	else if (bIsSeenPawn)
	{
		NewState = EFoeState::Attacking;
	}


	SetFoeState(NewState);
}

//Function to get out from stucking if foe stucked in some NavMesh
void ATDFoe::Stucking()
{
	FVector ReachablePoint = UNavigationSystemV1::GetRandomPointInNavigableRadius(GetWorld(), GetActorLocation(), 150);
	NextPathPoint = ReachablePoint;
	float DistanceToTarget = (GetActorLocation() - ReachablePoint).Size();
	if(DistanceToTarget <= RequiredDistanceToTarget)
	{
		bStuck = false;
	}
	
}

void ATDFoe::WalkingAround()
{
	FVector ReachablePoint = UNavigationSystemV1::GetRandomPointInNavigableRadius(GetWorld(), GetActorLocation()-FVector(25.f,25.f,25.f), 100);
	NextPathPoint = ReachablePoint;

}

//@todo need to implement all that attack mechanic
void ATDFoe::Attacking()
{
	
	
}

//Checking in what State foe is
void ATDFoe::SetFoeState(EFoeState NewState)
{

	const EFoeState PrevState = CurrentState;

	CurrentState = NewState;

	if (PrevState == EFoeState::Stucking && NewState == EFoeState::Stucking)
	{
		Stucking();
	}
	if (PrevState == EFoeState::Default && NewState == EFoeState::Attacking)
	{
		
	}
}



//Cheking is where a player around
void ATDFoe::OnNoiseHeard(APawn * NoiseInstigator, const FVector & Location, float Volume)
{
	
	if (DebugTDFoePathDrawing)
	{
		DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Red, false, 10.0f);
	}	
	
	bIsSeenPawn = true;
	TargetPawn = NoiseInstigator;
	NextPathPoint = GetNextPathPoint();
	DetermineFoeState();

}

//checking is foe overlap something
void ATDFoe::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	/*ATDBase* EnemyBase = Cast<ATDBase>(OtherActor);
	if (OtherActor == EnemyBase)
	{
		UGameplayStatics::ApplyDamage(OtherActor, 1.0f, GetController(), this, nullptr);
		this->Destroy();
		
	}*/
	
}


