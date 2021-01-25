// Fill out your copyright notice in the Description page of Project Settings.


#include "TDIdleNPC.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "TDGame/Public/TDPaperCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
ATDIdleNPC::ATDIdleNPC()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

		//Init the box and audio comps
	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("BoxComp"));
	BoxComp->SetupAttachment(GetRootComponent());

	AudioComp = CreateDefaultSubobject<UAudioComponent>(FName("AudioComp"));
	AudioComp->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ATDIdleNPC::BeginPlay()
{
	Super::BeginPlay();

	//Register the begin and end overlap functions
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ATDIdleNPC::OnBoxOverlap);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &ATDIdleNPC::OnBoxEndOverlap);
	
}

// Called every frame
void ATDIdleNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATDIdleNPC::OnBoxOverlap(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!IsPlayerOverlap)
	{
		if (OtherActor->IsA<ATDPaperCharacter>())
		{
			IsPlayerOverlap = true;
			ATDPaperCharacter* Char = Cast<ATDPaperCharacter>(OtherActor);
			Char->SetTalkRangeStatus(true);
			Char->GeneratePlayerLines(*PlayerLines);
			Char->SetAssociatedPawn(this);
		}
	}
	
}

void ATDIdleNPC::OnBoxEndOverlap(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<ATDPaperCharacter>())
	{
		IsPlayerOverlap = false;
		ATDPaperCharacter* Char = Cast<ATDPaperCharacter>(OtherActor);
		Char->SetTalkRangeStatus(false);
		Char->SetAssociatedPawn(nullptr);
	}
}



void ATDIdleNPC::Talk(USoundBase * SFX, TArray<FSubtitle> Subs)
{
	ATDPaperCharacter* Char = Cast<ATDPaperCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	//Play the corresponding sfx
	AudioComp->SetSound(SFX);
	AudioComp->Play();

	//Tell the UI to update with the new subtitles
	Char->GetUI()->UpdateSubtitles(Subs);
}



void ATDIdleNPC::AnswerToCharacter(FName PlayerLine, TArray<FSubtitle>& SubtitlesToDisplay, float delay)
{
	if (!AILines) return;

	//Retrieve the corresponding line
	FString ContextString;
	FDialog* Dialog = AILines->FindRow<FDialog>(PlayerLine, ContextString);

	ATDPaperCharacter* MainChar = Cast<ATDPaperCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Dialog && MainChar)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDel;

		TimerDel.BindUFunction(this, FName("Talk"), Dialog->SFX, Dialog->Subtitles);

		//Talk to the player after the delay time has passed
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, delay, false);
	}
}

