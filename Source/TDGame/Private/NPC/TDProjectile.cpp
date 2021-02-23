// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/TDProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "TDGame/Public/TDPaperCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

// Sets default values
ATDProjectile::ATDProjectile()
{
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> ProjectileAsset;
		FConstructorStatics()
			: ProjectileAsset(TEXT("PaperFlipbook'/Game/BluePrints/Foes/Effects/flameprojectile/Flame.Flame'"))
		{

		}
	};

	static FConstructorStatics ConstructorStatics;





	ProjectileAnimation = ConstructorStatics.ProjectileAsset.Get();


 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	HitBox = CreateDefaultSubobject<USphereComponent>(TEXT("HitBox"));
	HitBox->SetSphereRadius(8.0f);
	HitBox->SetGenerateOverlapEvents(true);
	HitBox->SetCanEverAffectNavigation(false);
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HitBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	HitBox->BodyInstance.bLockXRotation = true;
	HitBox->BodyInstance.bLockYRotation = true;
	HitBox->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	RootComponent = HitBox;



	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("SpriteComp"));
	FlipbookComponent->SetRelativeLocation(FVector(-1.0f, 0.0f, 5.0f));
	FlipbookComponent->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlipbookComponent->SetGenerateOverlapEvents(false);
	FlipbookComponent->SetCastShadow(false);
	FlipbookComponent->SetCanEverAffectNavigation(false);
	FlipbookComponent->SetupAttachment(GetRootComponent());

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(HitBox);
	ProjectileMovement->InitialSpeed = 100.0f;
	ProjectileMovement->MaxSpeed = 100.0f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

}

// Called when the game starts or when spawned
void ATDProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	this->SetLifeSpan(5.f);
}

void ATDProjectile::UpdateAnimation()
{
	FlipbookComponent->SetFlipbook(ProjectileAnimation);
}

// Called every frame
void ATDProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAnimation();
}


void ATDProjectile::NotifyActorBeginOverlap(AActor * OtherActor)
{
	ATDPaperCharacter* PlayerPawn = Cast<ATDPaperCharacter>(OtherActor);

	FHitResult hitRes;

	if (PlayerPawn)
	{
		Destroy();

		UGameplayStatics::ApplyPointDamage(PlayerPawn, int32(1),GetActorLocation(),hitRes, GetInstigatorController(),this, UDamageType::StaticClass());
	}


}