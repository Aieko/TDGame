// Fill out your copyright notice in the Description page of Project Settings.


#include "TDPaperCharacter.h"
#include "TDGame/TDGame.h"
#include "UObject/ConstructorHelpers.h"
#include "TDGame/Public/TDHealthComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "PaperFlipbook.h"
#include "TimerManager.h"
#include "TDFoe.h"
#include "Kismet/GameplayStatics.h"
#include "TDBase.h"




ATDPaperCharacter::ATDPaperCharacter(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	//Structures to load sprites right in the code
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> CharacterIdleAsset;
		FConstructorStatics()
			: CharacterIdleAsset(TEXT("PaperFlipbook'/Game/TDCharacter/PNG/Adventurer/Idle/Idle.Idle'"))
		{

		}
	};

	struct FConstructorStatics1
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> CharacterWalkAsset;
		FConstructorStatics1()
			: CharacterWalkAsset(TEXT("PaperFlipbook'/Game/TDCharacter/PNG/Adventurer/Walk/Adventurer_Sprite_Sheet_v1_1.Adventurer_Sprite_Sheet_v1_1'"))
		{

		}
	};

	struct FConstructorStatics2
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> CharacterAttackAsset1;
		FConstructorStatics2()
			: CharacterAttackAsset1(TEXT("PaperFlipbook'/Game/TDCharacter/PNG/Adventurer/Attack_2/Adventurer_Attack2_v1_1.Adventurer_Attack2_v1_1'"))
		{

		}
	};

	static FConstructorStatics ConstructorStatics;
	static FConstructorStatics1 ConstructorStatics1;
	static FConstructorStatics2 ConstructorStatics2;

	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCapsuleSize(8.5f, 12.0f);
	GetCapsuleComponent()->SetCanEverAffectNavigation(false);
	

	CharacterIdleAnimation = ConstructorStatics.CharacterIdleAsset.Get();
	CharacterWalkAnimation = ConstructorStatics1.CharacterWalkAsset.Get();
	CharacterAttackAnimation1 = ConstructorStatics2.CharacterAttackAsset1.Get();
	
	GetSprite()->SetRelativeLocation(FVector(0.0f, 0.0f, 5.0f));
	GetSprite()->SetCanEverAffectNavigation(false);

	//Creating HitBox with settings
	HitBox = CreateDefaultSubobject<USphereComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCanEverAffectNavigation(false);
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetSphereRadius(12.0f, true);
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ATDPaperCharacter::OnOverlap);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 220.f;
	CameraBoom->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	//GetCharacterMovement()->bConstrainToPlane = true;
	//GetCharacterMovement()->SetPlaneConstraintNormal(FVector(1.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	//GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	GetCharacterMovement()->MaxWalkSpeed = 120.f;

	//Saving original walk speed
	CharOrigMovementSpeed = GetCharacterMovement()->MaxWalkSpeed;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	//Health component of our char
	HealthComp = CreateDefaultSubobject<UTDHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ATDPaperCharacter::HandleTakeDamage);
	HealthComp->DefaultHealth = 6;
	//Attack Anim time
	AttackTime = 0.5f;
	//CD of Attack
	AttackCD = 0.3f;
	//Creating component which make foes see main char
	PawnNoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitterComp"));


	bIsTalking = false;
	bIsInTalkRange = false;
	AssociatedPawn = nullptr;

	AudioComp = CreateDefaultSubobject<UAudioComponent>(FName("AudioComp"));
	AudioComp->SetupAttachment(GetRootComponent());
}

void ATDPaperCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Casting to base. It gives a reference to ignore dd to our buildings
	Base = Cast<ATDBase>(Base);
}
//Determining states of char
void ATDPaperCharacter::DetermineState()
{
	ECharacterState NewState = ECharacterState::Default;

	if (bIsDead)
	{
		NewState = ECharacterState::Dead;
	}
	else
	{
		if (!bAttacking && !bIsDashing)
		{
			NewState = ECharacterState::Default;
		}
		else if (!bAttacking && !bIsDashing && bCanInteract)
		{
			NewState = ECharacterState::Interact;
		}
		if (bAttacking && ComboAttack == 1)
		{
			NewState = ECharacterState::AttackCombo1;
		}
		else if (bAttacking && ComboAttack == 2)
		{
			NewState = ECharacterState::AttackCombo2;
		}

		if (bIsDashing)
		{
			NewState = ECharacterState::Dashing;
		}


	}
	
	SetCharacterState(NewState);
}

void ATDPaperCharacter::SetCharacterState(ECharacterState NewState)
{
	const ECharacterState PrevState = CurrentState;

	CurrentState = NewState;
	if (NewState == ECharacterState::Dead)
	{
		bCanAttack = false;
		bCanDash = false;
		bCanInteract = false;
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		DisableInput(PlayerController);
	}

	if (PrevState == ECharacterState::Default && NewState == ECharacterState::Interact)
	{
		Interact();
	}
	
	if (NewState == ECharacterState::Dashing)
	{
		bCanAttack = false;
	}
	
}
//Main Animation State Machine
void ATDPaperCharacter::UpdateAnimation()
{
	if (!bIsDead)
	{
		//Setting Direction of sprite
		if (!bIsTurned)
		{
			GetSprite()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

		}
			else
			{
				GetSprite()->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));

			}
	//Checking if dashing first
	if (bIsDashing)
	{
		
		GetSprite()->SetFlipbook(CharacterDashAnimation);
		GetCharacterMovement()->MaxWalkSpeed *= 1.06f;
		//to move char when dashing
		MovingAction();
	}
	//checking if char gets hurt
		else if (bIsGetHit)
		{
			GetSprite()->SetLooping(false);
			GetSprite()->SetFlipbook(CharacterGetHitAnimation);
		

		}
		//If char don't hurt and not dashing then char can attack
			else if (bAttacking && ComboAttack == 1 && CurrentState == ECharacterState::AttackCombo1)
		{
			GetSprite()->SetLooping(false);
			GetSprite()->SetFlipbook(CharacterAttackAnimation1);
			//Change speed walk to be more slow
			GetCharacterMovement()->MaxWalkSpeed *= 0.8f;
			//to move char when attacking
			MovingAction();

		}
			//Second Combo
				else if (bAttacking && ComboAttack == 2 && CurrentState == ECharacterState::AttackCombo2)
			{
				GetSprite()->SetLooping(false);
				GetSprite()->SetFlipbook(CharacterAttackAnimation2);
				GetCharacterMovement()->MaxWalkSpeed *= 0.8f;
				MovingAction();
			}
				//If we don't do anything above then just play walk or idle animation 
					else
						{
							const FVector PlayerVelocity = GetVelocity();
							const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();
							// Are we moving or standing still?
							UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? CharacterWalkAnimation : CharacterIdleAnimation;

							GetSprite()->SetLooping(true);
							//Normalize walk speed
							if (GetCharacterMovement()->MaxWalkSpeed != CharOrigMovementSpeed)
							{
								GetCharacterMovement()->MaxWalkSpeed = CharOrigMovementSpeed;
							}
							//Setting idle or walking animation
							if (GetSprite()->GetFlipbook() != DesiredAnimation)
							{
							
								GetSprite()->SetFlipbook(DesiredAnimation);
							}
						}
	}
	//if char is dead then play death animation
	else
	{
		GetSprite()->SetFlipbook(CharacterDeathAnimation);
		GetSprite()->SetLooping(false);
	}
	
}
//updating character and making noise
void ATDPaperCharacter::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	MakeNoise(1.0, this);
}
//Move forward input
void ATDPaperCharacter::MoveForward(float Value)
{
	if ((Controller) && (Value != 0.0f) && !bIsTalking && CanMove)
	{
		bool IsLooping = GetSprite()->IsLooping();
		if (!IsLooping)
		{
			GetSprite()->SetLooping(true);
		}
		FRotator ControlRot = GetControlRotation();
		ControlRot.Pitch = 0.0f;
		ControlRot.Roll = 0.0f;

		// X = Forward (Red)
		// Y = Right (Green)
		// Z = Up (Blue)

		FVector ForwardVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

		AddMovementInput(ForwardVector, Value);
	}
}
//Move sideways input
void ATDPaperCharacter::MoveRight(float Value)
{
	if ((Controller) && (Value != 0.0f) && !bIsTalking && CanMove)
	{
		bool IsLooping = GetSprite()->IsLooping();
		if (!IsLooping)
		{
			GetSprite()->SetLooping(true);
		}

		FRotator ControlRot = GetControlRotation();
		ControlRot.Pitch = 0.0f;
		ControlRot.Roll = 0.0f;


		AddMovementInput(ControlRot.Vector(), Value);


		// Set the boolean so the character faces his Direction of travel.
		if (Value == 1.0f)
		{

			bIsTurned = false;

		}
		else if (Value == -1.0f)
		{

			bIsTurned = true;
		}
	}
}



//Attacking function
void ATDPaperCharacter::StartAttack()
{
	if (!bIsTalking)
	{
		//Setting ignored objects
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		if (Base)
		{
			IgnoredActors.Add(Base);
		}


		FVector CharAttackHitBox = GetActorLocation();

		if (bIsTurned)
		{

			CharAttackHitBox.Y += YAttackDeviation;
		}
		else
		{
			CharAttackHitBox.Y -= YAttackDeviation;
		}

		if (bCanAttack && ComboAttack == 1)
		{


			UGameplayStatics::ApplyRadialDamage(this, int32(1), CharAttackHitBox, 30.0f, nullptr, IgnoredActors, this, GetInstigatorController(), true);

			bCanAttack = false;

			GetWorldTimerManager().SetTimer(TimerHandle_AttackCD, this, &ATDPaperCharacter::ContinueAttack, AttackCD, false);

			GetWorldTimerManager().SetTimer(TimerHandle_AttackReset, this, &ATDPaperCharacter::ResetAttack, 1.2f, false);

			bAttacking = true;

			GetWorldTimerManager().SetTimer(TimerHandle_AttackAnimationTime, this, &ATDPaperCharacter::ResetAnimation, AttackTime, false);


			DetermineState();

		}
		else if (bCanAttack && ComboAttack == 2)
		{
			UGameplayStatics::ApplyRadialDamage(this, int32(1), CharAttackHitBox, 30.0f, nullptr, IgnoredActors, this, GetInstigatorController(), true);

			bCanAttack = false;

			GetWorldTimerManager().SetTimer(TimerHandle_AttackReset, this, &ATDPaperCharacter::ResetAttack, 0.5f, false);

			bAttacking = true;

			GetWorldTimerManager().SetTimer(TimerHandle_AttackAnimationTime, this, &ATDPaperCharacter::ResetAnimation, AttackTime, false);


			DetermineState();
		}

	}
	

}

void ATDPaperCharacter::MovingAction()
{

	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	if (bIsTurned)
	{
		AddMovementInput(ControlRot.Vector(), -1.0f);
	}
	else
	{
		AddMovementInput(ControlRot.Vector(), 1.0f);
	}

}


void ATDPaperCharacter::ResetAttack()
{

	bCanAttack = true;
	bAttacking = false;
	ComboAttack = 1;


	SetCharacterState(ECharacterState::Default);

}



void ATDPaperCharacter::ContinueAttack()
{

	bCanAttack = true;
	ComboAttack += 1;
	GetWorldTimerManager().SetTimer(TimerHandle_AttackReset, this, &ATDPaperCharacter::ResetAttack, 1.2f - AttackCD, false);
}

void ATDPaperCharacter::Dash()
{
	if (bCanDash)
	{
		bIsDashing = true;

		FRotator ControlRot = GetControlRotation();
		ControlRot.Pitch = 0.0f;
		ControlRot.Roll = 0.0f;

		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

		bCanAttack = false;
		
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		DisableInput(PlayerController);
		

		GetWorldTimerManager().SetTimer(TimerHandle_DashResetAnimation, this, &ATDPaperCharacter::ResetAnimation, 0.3f, false);

		bCanDash = false;

		GetWorldTimerManager().SetTimer(TimerHandle_ResetDashAbility, this, &ATDPaperCharacter::ResetDashAbility, 3.0f, false);

		DetermineState();
	}


}

void ATDPaperCharacter::ResetDashAbility()
{
	bCanDash = true;
	
	SetCharacterState(ECharacterState::Default);
}


void ATDPaperCharacter::ResetAnimation()
{
	if (bIsDashing)
	{
		bIsDashing = false;
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		bCanAttack = true;
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		EnableInput(PlayerController);
	}
	if (bAttacking)
	{
		bAttacking = false;
	}

	if (bIsGetHit)
	{
		bIsGetHit = false;
	}
}

void ATDPaperCharacter::Interact()
{
	bInteract = true;


}


void ATDPaperCharacter::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATDFoe* EnemyPawn = Cast<ATDFoe>(OtherActor);
	if (OtherActor)
	{
		
	}
}

void ATDPaperCharacter::HandleTakeDamage(UTDHealthComponent * OwningHealthComp, int32 Health, int32 HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Health <= 0)
	{
		bIsDead = true;
		DetermineState();
	}

	if (MatInst == nullptr)
	{
		MatInst = GetSprite()->CreateAndSetMaterialInstanceDynamicFromMaterial(0, GetSprite()->GetMaterial(0));
	}
	if (MatInst)

	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	StartImmortality();
	bIsGetHit = true;

	GetWorldTimerManager().SetTimer(TimerHandle_ResetAnimation, this, &ATDPaperCharacter::ResetAnimation, HurtTime, false);

}

void ATDPaperCharacter::StartImmortality()
{
	bIsImmortal = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	GetWorldTimerManager().SetTimer(TimerHandle_GetImmortality, this, &ATDPaperCharacter::ResetImmortality, 1.5f, false);

}

void ATDPaperCharacter::ResetImmortality()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	HitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	bIsImmortal = false;	

}
void ATDPaperCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCharacter();
}

void ATDPaperCharacter::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATDPaperCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATDPaperCharacter::MoveRight);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ATDPaperCharacter::StartAttack);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ATDPaperCharacter::Dash);


	InputComponent->BindAction("Talk", IE_Pressed, this, &ATDPaperCharacter::ToggleTalking);
}

void ATDPaperCharacter::ToggleTalking()
{
	if (bIsInTalkRange)
	{
		//If we are in talk range handle the talk status and the UI
		bIsTalking = !bIsTalking;
		ToggleUI();
		

	}
}

FDialog * ATDPaperCharacter::RetrieveDialog(UDataTable * TableToSearch, FName RowName)
{
	if (!TableToSearch) return nullptr;

	//The table is valid - retrieve the given row if possible
	FString ContextString;
	return TableToSearch->FindRow<FDialog>(RowName, ContextString);
}

void ATDPaperCharacter::GeneratePlayerLines(UDataTable & PlayerLines)
{
	//Get all the row names of the table
	TArray<FName> PlayerOptions = PlayerLines.GetRowNames();

	//For each row name try to retrieve the contents of the table
	for (auto It : PlayerOptions)
	{
		//Retrieve the contents of the table
		FDialog* Dialog = RetrieveDialog(&PlayerLines, It);

		if (Dialog)
		{
			//We retrieved a valid row - populate the questions array with our excerpts
			Questions.Add(Dialog->QuestionExcerpt);
		}
	}

	//Make sure to create a reference of the available line for later use
	AvailableLines = &PlayerLines;
}

void ATDPaperCharacter::Talk(FString Excerpt, TArray<FSubtitle>& Subtitles)
{
	//Get all the row names based on our stored lines
	TArray<FName> PlayerOptions = AvailableLines->GetRowNames();

	

	for (auto It : PlayerOptions)
	{
		//Search inside the available lines table to find the pressed Excerpt from the UI
		FDialog* Dialog = RetrieveDialog(AvailableLines, It);

		if (Dialog && Dialog->QuestionExcerpt == Excerpt)
		{
			//We've found the pressed excerpt - assign our sfx to the audio comp and play it
			AudioComp->SetSound(Dialog->SFX);
			AudioComp->Play();

			//Update the corresponding subtitles
			Subtitles = Dialog->Subtitles;


			if (UI && AssociatedPawn && Dialog->bShouldAIAnswer)
			{
				//Calculate the total displayed time for our subtitles
				//When the subtitles end - the associated pawn will be able to talk to our character

				TArray<FSubtitle> SubtitlesToDisplay;

				for (int32 i = 0; i < Subtitles.Num(); i++)
				{
					TotalSubsTime += Subtitles[i].AssociatedTime;
				}
				
				//Just a hardcoded value in order for the AI not to answer right after our subs.
				//It would be better if we expose that to our editor? Sure!
				//TotalSubsTime += 1.0f;
			

				//Tell the associated pawn to answer to our character after the specified time!
				AssociatedPawn->AnswerToCharacter(It, SubtitlesToDisplay, TotalSubsTime);

			}
			else if (!Dialog->bShouldAIAnswer) ToggleTalking();
			break;

		}
	}

}
