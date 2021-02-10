// Fill out your copyright notice in the Description page of Project Settings.


#include "TDGame/Public/InventorySystem/TDPickUp.h"
#include "Engine/Texture2D.h"
#include "TDPaperCharacter.h"

// Sets default values
UTDPickUp::UTDPickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	

	/*CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetSphereRadius(12.0f, true);
	CollisionSphere->SetCanEverAffectNavigation(false);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetRelativeRotation(FRotator(0.f, 90.f, -90.f));
	//RootComponent = CollisionSphere;

		//Initializing our properties
	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComp"));
	//SpriteComponent->SetupAttachment(GetRootComponent());
	SpriteComponent->SetCanEverAffectNavigation(false);
	SpriteComponent->SetGenerateOverlapEvents(false);
	SpriteComponent->SetCastShadow(false);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpriteComponent->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.6f));

	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FlipbookComp"));
	//FlipbookComponent->SetupAttachment(GetRootComponent());
	FlipbookComponent->SetCanEverAffectNavigation(false);
	FlipbookComponent->SetGenerateOverlapEvents(false);
	FlipbookComponent->SetCastShadow(false);
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	

	PressToPickUp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PickUpText"));
	PressToPickUp->SetText(FText::FromString(TEXT("Press F")));
	PressToPickUp->SetVisibility(false);
	//PressToPickUp->SetupAttachment(GetRootComponent());
	PressToPickUp->SetCanEverAffectNavigation(false);
	PressToPickUp->SetHorizontalAlignment(EHTA_Center);
	PressToPickUp->WorldSize = 13.f;
	PressToPickUp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PressToPickUp->SetRelativeLocation(FVector(0.000000,2.000000,6.000000));
	PressToPickUp->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	PressToPickUp->SetGenerateOverlapEvents(false);
	PressToPickUp->SetCastShadow(false);
	#include "PaperFlipbookComponent.h"
	#include "PaperFlipbook.h"
	#include "PaperSpriteComponent.h"
	#include "Components/TextRenderComponent.h"
	#include "Components/SphereComponent.h"
*/

	ItemName = FText::FromString("Item");
	UseActionText = FText::FromString("Use");

	PickupTexture = CreateDefaultSubobject<UTexture2D>(TEXT("ItemTexture"));
}

// Called when the game starts or when spawned





/*void UTDPickUp::SetGlowEffect(bool Status)
{
	if (this)
	{
		PressToPickUp->SetVisibility(Status);
	}
}*/

void UTDPickUp::Use(ATDPaperCharacter * Character)
{

}

