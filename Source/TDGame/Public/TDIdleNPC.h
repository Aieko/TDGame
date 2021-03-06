// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Engine/DataTable.h"
//#include "TDGame/Public/DialogSystem/Subtitles.h"
#include "GameFramework/Pawn.h"
#include "TDIdleNPC.generated.h"
UCLASS()
class TDGAME_API ATDIdleNPC : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDIdleNPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	UFUNCTION()
		void OnBoxOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnBoxEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, 
			class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:

	/*If the player is inside this box component he will be able to initiate a conversation with the pawn*/
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxComp;

	bool IsPlayerOverlap = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animations")
		class UPaperFlipbookComponent* FlipbookComponent;

	UPROPERTY(EditAnywhere, Category = "Animations")
		class UPaperFlipbook* NPCIdleAnimation;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DialogSystem)
		FText NPCName;

	/*   OLD DIALOGUE SYSTEM

	//The player lines - each pawn can offer different dialog options for our character
	UPROPERTY(EditAnywhere, Category = DialogSystem)
		UDataTable* PlayerLines;

	//The ai lines
	UPROPERTY(EditAnywhere, Category = DialogSystem)
		UDataTable* AILines;

	//The audio component responsible for playing the dialog coming from this pawn
	UPROPERTY(VisibleAnywhere)
		class UAudioComponent* AudioComp;

public:
	//Answers to the character after a specified delay
	void AnswerToCharacter(FName PlayerLine, TArray<FSubtitle>& SubtitlesToDisplay, float delay);

	//Retrieves the corresponding character lines
	UDataTable* GetPlayerLines() { return PlayerLines; }

private:

	UFUNCTION()
		void Talk(USoundBase* SFX, TArray<FSubtitle> Subs);

		*/


};
