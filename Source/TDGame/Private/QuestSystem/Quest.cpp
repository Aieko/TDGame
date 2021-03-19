// Fill out your copyright notice in the Description page of Project Settings.

#include "TDGame/Public/QuestSystem/Quest.h"
#include "TDGame/TDGame.h"
#include "TDGame/Public/QuestSystem/Objective.h"
#include "Sound/SoundCue.h"
#include "EngineUtils.h"

AQuest::AQuest() :
	QuestName(NAME_None),
	CurrentObjectives(),
	QuestStartDescription(),
	QuestEndDescription()
{

}


bool AQuest::IsQuestComplete() const
{
	
		bool result = true;
		for (auto Obj : CurrentObjectives)
		{
			if (!Obj)
			{
				UE_LOG(LogTemp, Warning, TEXT("Objective is invalid"));
				return false;
			}
			
			result &= Obj->IsComplete();
		}
		return result;
	
	
}



void AQuest::Update(FName Objective, int32 Progress)
{
	for (auto Obj : CurrentObjectives)
	{
		if (Obj->ObjectiveName == Objective)
		{
			if (!Obj)
			{
				UE_LOG(LogTemp, Warning, TEXT("Objective is invalid"));
				return;
			}

			Obj->Update(Progress);
			return;
		}
	}
}

bool AQuest::TryUpdate(FName Objective, int32 Progress)
{
	bool result = CanUpdate(Objective);
	if (result)
	{
		Update(Objective, Progress);
	}
	return result;
}
bool AQuest::CanUpdate(FName Objective)
{
	bool PreviousIsComplete = true;
	for (auto Obj : CurrentObjectives)
	{
		if (!Obj)
		{
			UE_LOG(LogTemp, Warning, TEXT("Objective is invalid"));
			return false;
		}

		if (PreviousIsComplete)
		{
			

			if (Objective == Obj->ObjectiveName)
				return true;
			else
				PreviousIsComplete = Obj->IsComplete() |
				!Obj->MustBeCompletedToAdvance;
		}
		else
		{
			return false;
		}
	}
	return true;
}

float AQuest::QuestCompletion() const
{
	int32 NumObjectives = CurrentObjectives.Num();
	if (NumObjectives == 0) return 1.0f;

	float AggregateCompletion = 0.0f;
	for (auto Obj : CurrentObjectives)
	{

		if (!Obj)
		{
			UE_LOG(LogTemp, Warning, TEXT("Objective is invalid"));
			return false;
		}

		AggregateCompletion += Obj->GetProgress();
	}
	return FMath::Clamp((AggregateCompletion / (float)NumObjectives),0.f, 1.f);
}