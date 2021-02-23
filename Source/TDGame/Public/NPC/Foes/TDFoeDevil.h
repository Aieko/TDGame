// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TDFoe.h"
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

	UPROPERTY(EditAnywhere, Category = "Animations")
		UPaperFlipbook* FoeAttackAnimation;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ATDProjectile>ProjectileClass;

	FTimerHandle TimerHandle_ResetAttack;

	virtual void BeginPlay() override;
};
