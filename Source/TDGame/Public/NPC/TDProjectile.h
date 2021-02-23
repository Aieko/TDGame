// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDProjectile.generated.h"

UCLASS()
class TDGAME_API ATDProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDProjectile();

	UPROPERTY(VisibleAnywhere, Category = Movement)
		class UProjectileMovementComponent* ProjectileMovement;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* HitBox;

	UPROPERTY(EditAnywhere, Category = "Animations")
	class	UPaperFlipbook* ProjectileAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animations")
	class	UPaperFlipbookComponent* FlipbookComponent;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void UpdateAnimation();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
