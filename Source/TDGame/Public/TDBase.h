// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDBase.generated.h"
class UTDHealthComponent;
class USphereComponent;
class UPaperSpriteComponent;
UCLASS()
class TDGAME_API ATDBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDBase();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTDHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animations")
	UPaperSpriteComponent* SpriteComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* CollisionComp;

	UFUNCTION()
	void HandleTakeDamage(UTDHealthComponent* OwningHealthComp, int32 Health, int32 HealthDelta, const class UDamageType* DamageType,
			class AController* InstigatedBy, AActor* DamageCauser);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
