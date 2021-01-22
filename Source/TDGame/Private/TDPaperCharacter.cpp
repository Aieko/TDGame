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


	HitBox = CreateDefaultSubobject<USphereComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCanEverAffectNavigation(false);
	HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	CharOrigMovementSpeed = GetCharacterMovement()->MaxWalkSpeed;


	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	HealthComp = CreateDefaultSubobject<UTDHealthComponent>(TEXT("HealthComp"));
	HealthComp->DefaultHealth = 6;

	AttackTime = 0.5f;

	AttackCD = 0.3f;

	PawnNoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitterComp"));
}

void ATDPaperCharacter::BeginPlay()
{
	Super::BeginPlay();

	Base = Cast<ATDBase>(Base);
}

void ATDPaperCharacter::DetermineState()
{
	ECharacterState NewState = ECharacterState::Default;


	if (!bAttacking)
	{
		NewState = ECharacterState::Default;
	}
	else if (!bAttacking && bInteract)
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

	SetCharacterState(NewState);

}

void ATDPaperCharacter::SetCharacterState(ECharacterState NewState)
{
	const ECharacterState PrevState = CurrentState;

	CurrentState = NewState;

	if (PrevState == ECharacterState::Default && NewState == ECharacterState::Interact)
	{
		Interact();
	}
	
	if (NewState == ECharacterState::Dashing)
	{
		bCanAttack = false;
	}
	
}

void ATDPaperCharacter::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	// Are we moving or standing still?
	
	//Setting Direction of sprite
	if (!bIsTurned)
	{
		GetSprite()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		
	}
	else
	{
		GetSprite()->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));

	}
	if (bAttacking && ComboAttack == 1 && CurrentState == ECharacterState::AttackCombo1)
	{
		GetSprite()->SetFlipbook(CharacterAttackAnimation1);
		GetCharacterMovement()->MaxWalkSpeed *= 0.8f;
		MoveWhenAttack();
		
	}
	else if (bAttacking && ComboAttack == 2 && CurrentState == ECharacterState::AttackCombo2)
	{
		GetSprite()->SetFlipbook(CharacterAttackAnimation2);
		GetCharacterMovement()->MaxWalkSpeed *= 0.8f;
		MoveWhenAttack();
	}
	else if (bIsDashing)
	{
		GetSprite()->SetFlipbook(CharacterDashAnimation);
		
	}
	else
	{

		UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? CharacterWalkAnimation : CharacterIdleAnimation;

		GetCharacterMovement()->MaxWalkSpeed = CharOrigMovementSpeed;

		if (GetSprite()->GetFlipbook() != DesiredAnimation)
		{
			GetSprite()->SetFlipbook(DesiredAnimation);
		}
	}
}

void ATDPaperCharacter::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	MakeNoise(1.0, this);
}

void ATDPaperCharacter::MoveForward(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	// X = Forward (Red)
	// Y = Right (Green)
	// Z = Up (Blue)

	FVector ForwardVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	AddMovementInput(ForwardVector, Value);

}

void ATDPaperCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;


	AddMovementInput(ControlRot.Vector(), Value);

	
	// Set the boolean so that the character faces his Direction of travel.
	if (Value==1.0f)
	{
		
		bIsTurned = false;
	
	}
	else if(Value==-1.0f)
	{
		
		bIsTurned = true;
	}
}
void ATDPaperCharacter::StartAttack()
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	if (Base)
	{
		IgnoredActors.Add(Base);
	}
	
	
	

	if (bCanAttack && ComboAttack == 1)
	{

		UGameplayStatics::ApplyRadialDamage(this, 50.0f, GetActorLocation(), 25.0f, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		bCanAttack = false;

		GetWorldTimerManager().SetTimer(TimerHandle_AttackCD, this, &ATDPaperCharacter::ContinueAttack, AttackCD, false);

		GetWorldTimerManager().SetTimer(TimerHandle_AttackReset, this, &ATDPaperCharacter::ResetAttack, 2.0f, false);

		bAttacking = true;

		GetWorldTimerManager().SetTimer(TimerHandle_AttackAnimationTime, this, &ATDPaperCharacter::ResetAnimation, AttackTime, false);

		
		DetermineState();

	}
	else if (bCanAttack && ComboAttack == 2)
	{
		UGameplayStatics::ApplyRadialDamage(this, 50.0f, GetActorLocation(), 25.0f, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		bCanAttack = false;

		GetWorldTimerManager().SetTimer(TimerHandle_AttackReset, this, &ATDPaperCharacter::ResetAttack, 1.0f, false);

		bAttacking = true;

		GetWorldTimerManager().SetTimer(TimerHandle_AttackAnimationTime, this, &ATDPaperCharacter::ResetAnimation, AttackTime, false);
		
		
		DetermineState();
	}

}

void ATDPaperCharacter::MoveWhenAttack()
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

void ATDPaperCharacter::Dash()
{
	if (bCanDash)
	{
		bIsDashing = true;
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		bCanAttack = false;
		FVector TeleportLoc = GetActorLocation();
		if (!bIsTurned)
		{
			TeleportLoc.Y -= 50.f;
		}
		else
		{
			TeleportLoc.Y += 50.f;
		}
		GetCapsuleComponent()->SetRelativeLocation(TeleportLoc);

		GetWorldTimerManager().SetTimer(TimerHandle_DashResetAnimation, this, &ATDPaperCharacter::ResetAnimation, 0.3f, false);

		bCanDash = false;

		GetWorldTimerManager().SetTimer(TimerHandle_ResetDashAbility, this, &ATDPaperCharacter::ResetDashAbility, 3.0f, false);
	}


}

void ATDPaperCharacter::ResetDashAbility()
{
	bCanDash = true;
}



void ATDPaperCharacter::ResetAttack()
{
	
	bCanAttack = true;
	bAttacking = false;
	ComboAttack = 1;

	
	SetCharacterState(ECharacterState::Default);
	
}

void ATDPaperCharacter::ResetAnimation()
{
	if (bIsDashing)
	{
		bIsDashing = false;
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		DetermineState();
		bCanAttack = true;
	}
	if (bAttacking)
	{
		bAttacking = false;
	}
}

void ATDPaperCharacter::ContinueAttack()
{

	bCanAttack = true;
	ComboAttack += 1;
	GetWorldTimerManager().SetTimer(TimerHandle_AttackReset, this, &ATDPaperCharacter::ResetAttack, 2.0f - AttackCD, false);
}

void ATDPaperCharacter::Interact()
{

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
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		bIsDead = true;
		this->DisableInput(PlayerController);
	}
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
}
