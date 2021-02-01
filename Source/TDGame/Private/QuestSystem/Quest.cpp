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

void AQuest::BeginPlay()
{
	Super::BeginPlay();
	FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
		for (auto Objective : Objectives)
		{

			//CurrentObjectives.Add(GetWorld()->SpawnActor<AObjective>(Location,Rotation, SpawnInfo));

			//TActorIterator<AObjective> ExistObjectives(GetWorld(), AObjective::StaticClass());
			for (TActorIterator<AObjective> It(GetWorld(), AObjective::StaticClass()); It; ++It)
			{
				AObjective* ExistObjective = *It;
				if (ExistObjective)
				{
					CurrentObjectives.Add(ExistObjective);
				}
			}
		}
	
}

// Called every frame
void AQuest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AQuest::IsQuestComplete() const
{
	bool result = true;
	for (auto Objective : CurrentObjectives)
	{
		result &= Objective->IsComplete();
	}
	return result;
}



void AQuest::Update(FName Objective, int32 Progress)
{
	for (auto Obj : CurrentObjectives)
	{
		if (Obj->ObjectiveName == Objective)
		{
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
	for (auto Objective : CurrentObjectives)
	{
		AggregateCompletion += Objective->GetProgress();
	}
	return FMath::Clamp((AggregateCompletion / (float)NumObjectives),0.f, 1.f);
}