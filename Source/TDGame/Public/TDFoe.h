// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TDFoe.generated.h"
enum class EFoeState
{
	MovingToBase,
	Stucking,
	Attacking
};
class UFloatingPawnMovement;
class UPawnSensingComponent;
class UTDHealthComponent;
class UPawnNoiseEmitterComponent;
class USphereComponent;
class UPaperFlipbookComponent;
class UPaperFlipbook;
UCLASS()
class TDGAME_API ATDFoe : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATDFoe();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTDHealthComponent* HealthComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "QuestComponents")
	class AObjective* AttachedObjective;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UFloatingPawnMovement* MovementComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animations")
		UPaperFlipbookComponent* FlipbookComponent;

	UPROPERTY(EditAnywhere, Category = "Animations")
		UPaperFlipbook* FoeIdleAnimation;

	UPROPERTY(EditAnywhere, Category = "Animations")
		UPaperFlipbook* FoeWalkAnimation;

	UPROPERTY(EditAnywhere, Category = "Animations")
		UPaperFlipbook* DeathAnimation;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		USphereComponent* HitBox;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnNoiseEmitterComponent* PawnNoiseEmitterComp;
	//						*******NAVIGATION******
	
	UPROPERTY(EditAnywhere)
	AActor* Base;

	void UpdateAnimation();

	void UpdateFoe();

	bool bIsTurned = false;

	APawn* TargetPawn;

	FVector GetNextPathPoint();

	//Next PathPoint in Navigation path
	FVector NextPathPoint;

	FVector LastFoeLocation;

	FVector PawnReturnLocation;

	void Stucking();

	void Attacking();

	void DetermineFoeState();

	EFoeState CurrentState;

	void SetFoeState(EFoeState NewState);

	bool bIsSeenPawn = false;

	bool bReturnToPath = false;

	bool bStuck = false;

	float ZLoc;

	FTimerHandle TimerHandle_RefreshPath;



	UPROPERTY(EditDefaultsOnly, Category = "Foe")
		float RequiredDistanceToTarget;
	UPROPERTY(EditDefaultsOnly, Category = "Foe")
		float DistanceToBreakChasing;

	class ATDPaperCharacter* Player;
	
	//						******PAWN SENSING******

	UFUNCTION()
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

	FRotator OriginalRotation;
	//				  ******HANDLE DAMAGE & MAKE DAMAGE******
	UFUNCTION()
	void HandleTakeDamage(UTDHealthComponent* OwningHealthComp, int32 Health, int32 HealthDelta, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);

	UMaterialInstanceDynamic* MatInst;

	bool bIsDead = false;
public:	

	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RefreshPath();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
