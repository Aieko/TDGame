// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "TDPaperCharacter.generated.h"
enum class ECharacterState
{
	Default,
	Interact,
	AttackCombo1,
	AttackCombo2,
	Dashing
};
class UPaperFlipbookComponent;
class UPaperFlipbook;
class UTDHealthComponent;
class UPawnNoiseEmitterComponent;
class USphereComponent;
UCLASS()
class TDGAME_API ATDPaperCharacter : public APaperCharacter
{
	GENERATED_UCLASS_BODY()


public:



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UTDHealthComponent* HealthComp;

	
private:

	UPROPERTY(EditAnywhere, Category = "Animations")
		UPaperFlipbook* CharacterIdleAnimation;

	UPROPERTY(EditAnywhere, Category = "Animations")
		UPaperFlipbook* CharacterWalkAnimation;

	UPROPERTY(EditAnywhere, Category = "Animations")
		UPaperFlipbook* CharacterAttackAnimation1;

	UPROPERTY(EditAnywhere, Category = "Animations")
		UPaperFlipbook* CharacterAttackAnimation2;

	UPROPERTY(EditAnywhere, Category = "Animations")
		UPaperFlipbook* CharacterDashAnimation;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void DetermineState();

	ECharacterState CurrentState;

	void SetCharacterState(ECharacterState NewState);

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	void UpdateCharacter();

	bool bIsDead = false;

	bool bIsTurned = false;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* HitBox;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnNoiseEmitterComponent* PawnNoiseEmitterComp;

	void MoveForward(float Value);

	void MoveRight(float Value);
	UFUNCTION()
	void ResetAttack();

	UFUNCTION()
	void StartAttack();

	UFUNCTION()
	void ContinueAttack();
	UFUNCTION()
	void Interact();

	bool bCanAttack = true;

	bool bAttacking = false;

	int32 ComboAttack = 1;

	bool bInteract = false;

	FTimerHandle TimerHandle_AttackCD;

	FTimerHandle TimerHandle_AttackReset;

	FTimerHandle TimerHandle_AttackAnimationTime;

	FTimerHandle TimerHandle_MovingThenAttack;

	FTimerHandle TimerHandle_DashResetAnimation;

	FTimerHandle TimerHandle_ResetDashAbility;

	UPROPERTY(EditAnywhere, Category = "GamePlay")
		float AttackTime;

	UPROPERTY(EditAnywhere, Category = "GamePlay")
		float AttackCD;

	UFUNCTION()
		void ResetAnimation();

	void MoveWhenAttack();

	void Dash();

	UFUNCTION()
	void ResetDashAbility();

	bool bCanDash = true;

	bool bIsDashing;

	float CharOrigMovementSpeed;

	UPROPERTY(EditAnywhere, Category = "GamePlay")
	AActor* Base;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void HandleTakeDamage(UTDHealthComponent* OwningHealthComp, int32 Health, int32 HealthDelta, const class UDamageType* DamageType,
			class AController* InstigatedBy, AActor* DamageCauser);
public:

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
