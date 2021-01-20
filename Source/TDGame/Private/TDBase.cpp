// Fill out your copyright notice in the Description page of Project Settings.


#include "TDGame/Public/TDBase.h"
#include "TDGame/Public/TDHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Paper2D/Classes/PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "UObject/ConstructorHelpers.h"
// Sets default values
ATDBase::ATDBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperSprite> SpriteComponentAsset;
		FConstructorStatics()
			: SpriteComponentAsset(TEXT("PaperSprite'/Game/TileSets/Buildings/Bases/building_1_Sprite.building_1_Sprite'"))
		{

		}
	};

	static FConstructorStatics ConstructorStatics;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetSphereRadius(54);
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	RootComponent = CollisionComp;

		SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComp"));
		SpriteComponent->SetSprite(ConstructorStatics.SpriteComponentAsset.Get());
		SpriteComponent->SetupAttachment(GetRootComponent());
	
	

	HealthComp = CreateDefaultSubobject<UTDHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ATDBase::HandleTakeDamage);
	HealthComp->DefaultHealth = 10;
}

void ATDBase::HandleTakeDamage(UTDHealthComponent * OwningHealthComp, float Health, float HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{

}

// Called when the game starts or when spawned
void ATDBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATDBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

