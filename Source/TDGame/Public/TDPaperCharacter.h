// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "TDGame/Public/DIalogUI.h"
#include "TDGame/Public/TDIdleNPC.h"
#include "PaperCharacter.h"
#include "TDPaperCharacter.generated.h"
enum class ECharacterState
{
	Default,
	Interact,
	AttackCombo1,
	AttackCombo2,
	Dashing,
	Dead
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
	//Animations
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

	UPROPERTY(EditAnywhere, Category = "Animations")
		UPaperFlipbook* CharacterGetHitAnimation;

	UPROPERTY(EditAnywhere, Category = "Animations")
		UPaperFlipbook* CharacterDeathAnimation;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Second collision sphere
	UPROPERTY(VisibleAnywhere, Category = "Components")
		USphereComponent* HitBox;
	//component which allow chracter make noises
	UPROPERTY(VisibleAnywhere, Category = "Components")
		UPawnNoiseEmitterComponent* PawnNoiseEmitterComp;
	//Determining and setting state fuctions
	void DetermineState();

	ECharacterState CurrentState;

	void SetCharacterState(ECharacterState NewState);

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	UFUNCTION()
	void ResetAnimation();

	void UpdateCharacter();

	bool bIsDead = false;

	bool bIsTurned = false;

	// Moving functions
	void MoveForward(float Value);

	void MoveRight(float Value);
	//Attack functions

	UFUNCTION()
	void StartAttack();

	UFUNCTION()
	void ResetAttack();

	bool bCanAttack = true;

	bool bAttacking = false;

	UFUNCTION()
	void ContinueAttack();

	int32 ComboAttack = 1;

	UPROPERTY(EditAnywhere, Category = "Animation")
		float AttackTime;

	UPROPERTY(EditAnywhere, Category = "Animation")
	float HurtTime;

	UPROPERTY(EditAnywhere, Category = "GamePlay")
		float AttackCD;
	UPROPERTY(EditAnywhere, Category = "Animations")
	float YAttackDeviation = 10.0f;
	//To make main character move when attacking
	void MovingAction();


	//Saving original movement speed
	float CharOrigMovementSpeed;

	//Interact function
	UFUNCTION()
	void Interact();

	bool bInteract = false;

	bool bCanInteract = true;
	//Timer handles which are making cd for main character
	FTimerHandle TimerHandle_AttackCD;

	FTimerHandle TimerHandle_AttackReset;

	FTimerHandle TimerHandle_AttackAnimationTime;

	FTimerHandle TimerHandle_MovingThenAttack;

	FTimerHandle TimerHandle_DashResetAnimation;

	FTimerHandle TimerHandle_ResetAnimation;

	FTimerHandle TimerHandle_ResetDashAbility;

	FTimerHandle TimerHandle_GetImmortality;

	//Dash ability
	void Dash();

	UFUNCTION()
	void ResetDashAbility();

	bool bCanDash = true;

	bool bIsDashing;

	void StartImmortality();

	bool bIsGetHit = false;

	bool bIsImmortal = false;

	UFUNCTION()
	void ResetImmortality();
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	AActor* Base;
	//Functions which calls when main character overlaping someone
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//function which calls when main character gets hurt
	UFUNCTION()
		void HandleTakeDamage(UTDHealthComponent* OwningHealthComp, int32 Health, int32 HealthDelta, const class UDamageType* DamageType,
			class AController* InstigatedBy, AActor* DamageCauser);

	class UMaterialInstanceDynamic* MatInst;
public:

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//						***********DIALOG SYSTEM***********




private:
	/*True if the player is currently talking with any pawn*/
	bool bIsTalking;

	/*True if the player is inside a valid range to start talking to a pawn*/
	bool bIsInTalkRange;

	/*Initiates or terminates a conversation*/
	void ToggleTalking();

	/*The pawn that the player is currently talking to*/
	ATDIdleNPC* AssociatedPawn;

	/*A reference to our lines - retrieved from the associated pawn*/
	UDataTable* AvailableLines;

	/*Searches in the given row inside the specified table*/
	FDialog* RetrieveDialog(UDataTable* TableToSearch, FName RowName);

public:

	/*Generates the player lines*/
	void GeneratePlayerLines(UDataTable& PlayerLines);

	/*This array is essentially an Array of Excerpts from our dialogs!*/
	UPROPERTY(BlueprintReadOnly)
		TArray<FString> Questions;

	/*Performs the actual talking - informs the associated pawn if necessary in order to answer
	The subtitles array contain all the subtitles for this talk - it should be passed to our UI*/
	UFUNCTION(BlueprintCallable, Category = DialogSystem)
		void Talk(FString Excerpt, TArray<FSubtitle>& Subtitles);

	/*Enables / disables our talk ability. The player can't talk if he's not in a valid range*/
	void SetTalkRangeStatus(bool Status) { bIsInTalkRange = Status; }

	/*Sets a new associated pawn*/
	void SetAssociatedPawn(ATDIdleNPC* Pawn) { AssociatedPawn = Pawn; }

	/*Retrieves the UI reference*/
	UDIalogUI* GetUI() { return UI; }

protected:
	UPROPERTY(EditAnywhere, Category = DialogSystem)
	float TotalSubsTime = 0.f;

	/*The component responsible for playing our SFX*/
	UPROPERTY(VisibleAnywhere)
		UAudioComponent* AudioComp;

	/*Opens or closes the conversation UI*/
	UFUNCTION(BlueprintImplementableEvent, Category = DialogSystem)
		void ToggleUI();

	/*UI Reference*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UDIalogUI* UI;

};
