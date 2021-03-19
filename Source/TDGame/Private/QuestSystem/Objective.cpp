// Fill out your copyright notice in the Description page of Project Settings.
#include "TDGame/Public/QuestSystem/Objective.h"
#include "TDGame/TDGame.h"


// Sets default values
AObjective::AObjective() :
	Description(),
	ObjectiveName(NAME_None),
	TotalProgressNeeded(1),
	CurrentProgress(0),
	MustBeCompletedToAdvance(true)

{


}


void AObjective::Update(int32 Progress)
{
	CurrentProgress += Progress;
}

bool AObjective::IsComplete() const
{
	return CurrentProgress >= TotalProgressNeeded;
}

float AObjective::GetProgress() const
{
	check(TotalProgressNeeded != 0)
		return 	FMath::Clamp(((float)CurrentProgress / (float)TotalProgressNeeded),0.f,1.f);
}
