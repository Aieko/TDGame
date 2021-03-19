// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TDFoe.h"
#include "TDMinion.h"
#include "TDFoeDevil.generated.h"
class ATDProjectile;
/**
 * 
 */
UCLASS()
class TDGAME_API ATDFoeDevil : public ATDFoe
{
	GENERATED_BODY()
	
public:

	ATDFoeDevil();

	void Attacking() override;

	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume) override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UArrowComponent* ArrowComp;

	void Tick(float DeltaTime) override;



protected:
	class USphereComponent* MinionsZoneSphere;

	UFUNCTION()
	void OnMinionsEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, Category = "Animations")
	UPaperFlipbook* FoeAttackAnimation;

	UPROPERTY(EditDefaultsOnly, Category = Devil)
	TSubclassOf<class ATDProjectile>ProjectileClass;

	FTimerHandle TimerHandle_ResetAttack;

	FTimerHandle TimerHandle_ResetSummon;

	virtual void BeginPlay() override;

	bool bCanSummon = true;

private:

	TArray<class ATDMinion*> MinionHorde;

	UPROPERTY(EditAnywhere, Category = "Devil")
	int32 MaxMinions;

	int32 MinionsAlive;

	UPROPERTY(EditDefaultsOnly, Category = Devil)
	TSubclassOf<class ATDMinion>MinionClass;

	void SpawnMinion();
};
