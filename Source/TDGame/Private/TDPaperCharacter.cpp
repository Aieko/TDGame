// Fill out your copyright notice in the Description page of Project Settings.


#include "TDPaperCharacter.h"
#include "TDGame/TDGame.h"
#include "UObject/ConstructorHelpers.h"
#include "TDGame/Public/TDHealthComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "PaperFlipbookComponent.h"
#include "TDGame/TDGamePlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "PaperFlipbook.h"
#include "TimerManager.h"
#include "TDFoe.h"
#include "TDBase.h"
#include "Kismet/GameplayStatics.h"
#include "TDGame/Public/UIJournal.h"
#include "TDGame/Public/InventorySystem/TDPickUp.h"
#include "TDGame/Public/InventorySystem/TDInventoryComponent.h"
#include "NPC/TDProjectile.h"


ATDPaperCharacter::ATDPaperCharacter(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	//Structures to load sprites right in the code
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> CharacterIdleAsset;
		FConstructorStatics()
			: CharacterIdleAsset(TEXT("PaperFlipbook'/Game/TDCharacter/PNG/KnightCharacter/Idle/HeroKnight_Idle.HeroKnight_Idle'"))
		{

		}
	};

	struct FConstructorStatics1
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> CharacterWalkAsset;
		FConstructorStatics1()
			: CharacterWalkAsset(TEXT("PaperFlipbook'/Game/TDCharacter/PNG/KnightCharacter/Run/HeroKnight_Run.HeroKnight_Run'"))
		{

		}
	};

	struct FConstructorStatics2
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> CharacterAttackAsset1;
		FConstructorStatics2()
			: CharacterAttackAsset1(TEXT("PaperFlipbook'/Game/TDCharacter/PNG/KnightCharacter/Attack1/HeroKnight_Attack1.HeroKnight_Attack1'"))
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
	GetSprite()->SetLooping(true);
	

	//Creating HitBox with settings
	HitBox = CreateDefaultSubobject<USphereComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCanEverAffectNavigation(false);
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetSphereRadius(12.0f, true);
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ATDPaperCharacter::OnOverlap);

	PickUpSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpSphere"));
	PickUpSphere->SetupAttachment(RootComponent);
	PickUpSphere->SetCanEverAffectNavigation(false);
	PickUpSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickUpSphere->SetSphereRadius(36.0f, true);
	PickUpSphere->OnComponentBeginOverlap.AddDynamic(this, &ATDPaperCharacter::PickUpOverlap);
	PickUpSphere->OnComponentEndOverlap.AddDynamic(this, &ATDPaperCharacter::PickUpOverlapEnd);

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

	//CD of Attack
	AttackCD = 0.3f;
	//Creating component which make foes see main char
	PawnNoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitterComp"));

	//InventoryComponent = CreateDefaultSubobject<UTDInventoryComponent>(TEXT("Inventory"));
	//InventoryComponent->Capacity = 12;

	bIsTalking = false;
	bIsInTalkRange = false;
	AssociatedPawn = nullptr;

	AudioComp = CreateDefaultSubobject<UAudioComponent>(FName("AudioComp"));
	AudioComp->SetupAttachment(GetRootComponent());
}

void ATDPaperCharacter::PickUpOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	
	

}

void ATDPaperCharacter::PickUpOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	
	
}


void ATDPaperCharacter::PickupItem()
{
	/*if (LastItemSeen)
	{
		//Find the first available slot
	//	int32 AvailableSlot = Inventory.Find(nullptr);

		if (AvailableSlot != INDEX_NONE)
		{
			//Add the item to the first valid slot we found
			//Inventory[AvailableSlot] = LastItemSeen;
			//Destroy the item from the game
			LastItemSeen->Destroy();
		}
		else GLog->Log("You can't carry any more items!");
	}*/
}

void ATDPaperCharacter::UseItem(class UTDPickUp* Item)
{
	if (Item)
	{
		Item->Use(this);
		Item->OnUse(this); // Blueprint
	}
}

void ATDPaperCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Casting to base. It gives a reference to ignore dd to our buildings
	Base = Cast<ATDBase>(Base);
	LastItemSeen = nullptr;

	CurrentHealth = HealthComp->GetHealth();
	
	//GetSprite()->OnFinishedPlaying.AddDynamic(this, &ATDPaperCharacter::UpdateAnimation);
	
	
}

void ATDPaperCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCharacter();
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

		if (bIsBlocking)
		{
			NewState = ECharacterState::Blocking;
		}

		if (bIsGetHit)
		{
			NewState = ECharacterState::Hurt;
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

	if (NewState == ECharacterState::Default)
	{
		this->CanMove = true;
		bCanBlock = true;
	}
	
	if (NewState == ECharacterState::Dashing)
	{
		bCanAttack = false;
	}

	if (NewState == ECharacterState::Blocking)
	{
		this->CanMove = false;
		this->SetCanBeDamaged(false);

		
	}

}
//Main Animation State Machine
void ATDPaperCharacter::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();
	
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
		if (CurrentState == ECharacterState::Dashing)
		{

			GetSprite()->SetFlipbook(CharacterDashAnimation);
			GetCharacterMovement()->MaxWalkSpeed *= 1.06f;
			
			
			//to move char when dashing
			MovingAction();
		}

			else if (CurrentState == ECharacterState::Blocking && !bIsBlocked)
			{
				GetSprite()->SetFlipbook(CharacterBlockAnimation);
			}
			else if(bIsBlocked)
				{
				GetSprite()->SetFlipbook(CharacterBlockedAnimation);
				}
				//checking if char gets hurt
				else if (CurrentState == ECharacterState::Hurt)
				{
					
					GetSprite()->SetFlipbook(CharacterGetHitAnimation);
					if (GetSprite()->IsLooping())
					{
						//GetSprite()->SetLooping(false);
					}

				}
					//If char don't hurt and not dashing then char can attack
					else if (bAttacking && ComboAttack == 1 && CurrentState == ECharacterState::AttackCombo1)
					{
						
						GetSprite()->SetFlipbook(CharacterAttackAnimation1);
						//Change speed walk to be slow
						GetCharacterMovement()->MaxWalkSpeed *= 0.8f;
						//to move char when attacking
						MovingAction();

					}
					//Second Combo
					else if (bAttacking && ComboAttack == 2 && CurrentState == ECharacterState::AttackCombo2)
					{

							
						GetSprite()->SetFlipbook(CharacterAttackAnimation2);
						GetCharacterMovement()->MaxWalkSpeed *= 0.8f;
						MovingAction();
					}
						//If we don't do anything above then just play walk or idle animation 
						else if((PlayerSpeedSqr > 0.0f) && CurrentState == ECharacterState::Default)
						{
								if (!GetSprite()->IsLooping())
								{
									GetSprite()->SetLooping(true);
								}

								GetSprite()->SetFlipbook(CharacterWalkAnimation);
					
								//Normalize walk speed
								if (GetCharacterMovement()->MaxWalkSpeed != CharOrigMovementSpeed)
								{
									GetCharacterMovement()->MaxWalkSpeed = CharOrigMovementSpeed;
								}

						}
							else if(CurrentState == ECharacterState::Default)
								{
									if (!GetSprite()->IsLooping())
									{
										GetSprite()->SetLooping(true);
									}

									GetSprite()->SetFlipbook(CharacterIdleAnimation);

									//Normalize walk speed
									if (GetCharacterMovement()->MaxWalkSpeed != CharOrigMovementSpeed)
									{
										GetCharacterMovement()->MaxWalkSpeed = CharOrigMovementSpeed;
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
	if ((Controller) && (Value != 0.0f) && !bIsTalking && CanMove && !bIsDashing)
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
}
//Move sideways input
void ATDPaperCharacter::MoveRight(float Value)
{
	if ((Controller) && (Value != 0.0f) && !bIsTalking && CanMove && !bIsDashing)
	{
		
		

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
	if (!bIsTalking&&!bIsBlocking)
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
			
			bCanBlock = false;

			GetWorldTimerManager().SetTimer(TimerHandle_AttackCD, this, &ATDPaperCharacter::ContinueAttack,
				float((CharacterAttackAnimation1->GetNumFrames()-3) / CharacterAttackAnimation1->GetFramesPerSecond()), false);

			GetWorldTimerManager().SetTimer(TimerHandle_AttackReset, this, &ATDPaperCharacter::ResetAttack, 1.2f, false);

			bAttacking = true;

			GetWorldTimerManager().SetTimer(TimerHandle_AttackAnimationTime, this, &ATDPaperCharacter::ResetAnimation,
				float((CharacterAttackAnimation1->GetNumFrames()-1)/CharacterAttackAnimation1->GetFramesPerSecond()), false);

			float AnimationDuration = CharacterAttackAnimation1->GetNumFrames() / CharacterAttackAnimation1->GetFramesPerSecond();
			DetermineState();

		}
		else if (bCanAttack && ComboAttack == 2)
		{
			UGameplayStatics::ApplyRadialDamage(this, int32(1), CharAttackHitBox, 30.0f, nullptr, IgnoredActors, this, GetInstigatorController(), true);

			bCanAttack = false;
		
			bCanBlock = false;

			GetWorldTimerManager().SetTimer(TimerHandle_AttackReset, this, &ATDPaperCharacter::ResetAttack, 0.5f, false);

			bAttacking = true;

			GetWorldTimerManager().SetTimer(TimerHandle_AttackAnimationTime, this, &ATDPaperCharacter::ResetAnimation,
				float((CharacterAttackAnimation2->GetNumFrames() - 1) / CharacterAttackAnimation2->GetFramesPerSecond()), false);


			DetermineState();
		}

	}
	

}

void ATDPaperCharacter::StartBlock()
{
	if (bCanBlock)
	{
		bIsBlocking = true;
		
	}

	DetermineState();
	
}

void ATDPaperCharacter::StopBlock()
{
	this->CanMove = true;
	this->SetCanBeDamaged(true);
	bIsBlocking = false;
	bCanAttack = true;

	DetermineState();
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
	
	bCanBlock = true;
	ComboAttack = 1;

}



void ATDPaperCharacter::ContinueAttack()
{

	bCanAttack = true;
	
	bCanBlock = true;
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
		bAttacking = false;
		bCanBlock = false;

		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		DisableInput(PlayerController);
		

		GetWorldTimerManager().SetTimer(TimerHandle_DashResetAnimation, this, &ATDPaperCharacter::ResetAnimation,
			0.3f, false);

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
		bCanBlock = true;
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		EnableInput(PlayerController);
	}
	if (bAttacking)
	{
		bAttacking = false;

		UE_LOG(LogTemp, Warning, TEXT("Bool value is: %s"), bAttacking ? TEXT("true") : TEXT("false"));
		
	}

	if (bIsGetHit)
	{
		bIsGetHit = false;	
		bCanAttack = true;
		bCanBlock = true;
	}

	if (bIsBlocked)
	{
		bIsBlocked = false;
	}

	DetermineState();
}

void ATDPaperCharacter::Interact()
{
	bInteract = true;


}


void ATDPaperCharacter::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATDProjectile* Projectile = Cast<ATDProjectile>(OtherActor);
	if (OtherActor&&bIsBlocking&&!bIsBlocked)
	{
		bIsBlocked = true;

		GetWorldTimerManager().SetTimer(TimerHandle_AttackAnimationTime, this, &ATDPaperCharacter::ResetAnimation,
			float((CharacterBlockedAnimation->GetNumFrames() - 1) / CharacterBlockedAnimation->GetFramesPerSecond()), false);
	}
}

void ATDPaperCharacter::HandleTakeDamage(UTDHealthComponent * OwningHealthComp, int32 Health, int32 HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	
	if (CurrentHealth < Health || CurrentHealth == Health)
	{
		CurrentHealth = Health;
	}
	else
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
		bCanAttack = false;
		
		bCanBlock = false;

		GetWorldTimerManager().SetTimer(TimerHandle_ResetAnimation, this, &ATDPaperCharacter::ResetAnimation,
			CharacterGetHitAnimation->GetTotalDuration() - 0.1f, false);

		CurrentHealth = Health;

		DetermineState();
	}
		

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


void ATDPaperCharacter::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATDPaperCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATDPaperCharacter::MoveRight);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ATDPaperCharacter::StartAttack);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ATDPaperCharacter::Dash);


	InputComponent->BindAction("Talk", IE_Pressed, this, &ATDPaperCharacter::ToggleTalking);
	InputComponent->BindAction("Journal", IE_Pressed, this, &ATDPaperCharacter::ToggleJournal);


	//Action mapping of pickup item
	InputComponent->BindAction("Pickup", IE_Pressed, this, &ATDPaperCharacter::PickupItem);

	InputComponent->BindAction("Inventory", IE_Pressed, this, &ATDPaperCharacter::ToggleInventory);

	InputComponent->BindAction("Block", IE_Pressed, this, &ATDPaperCharacter::StartBlock);
	InputComponent->BindAction("Block", IE_Released, this, &ATDPaperCharacter::StopBlock);

	
}

void ATDPaperCharacter::ToggleTalking()
{
	if (bIsInTalkRange)
	{
		//If we are in talk range handle the talk status and the UI
		
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

void ATDPaperCharacter::ToggleJournal()
{
	if (!bIsTalking)
	{
		ToggleJournalUI();
	}
	
}

void ATDPaperCharacter::ToggleInventory()

{
	
		ToggleInventoryUI();
	
	
}
