// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDGame/Public/DialogSystem/Dialog.h"
#include "DIalogUI.generated.h"

/**
 * 
 */
UCLASS()
class TDGAME_API UDIalogUI : public UUserWidget
{
	GENERATED_BODY()
	

public:

	/*This property will be used in order to bind our subtitles
	Binding will make sure to notify the UI if the content of the following
	variable change.*/
	UPROPERTY(BlueprintReadOnly)
		FString SubtitleToDisplay;


	/*Updates the displayed subtitles based on the given array*/
	UFUNCTION(BlueprintCallable, Category = "DialogSystem")
		void UpdateSubtitles(TArray<FSubtitle> Subtitles);

	/*This array will populate our buttons from within the show function*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<FString> Questions;

	/*Adds the widget to our viewport and populates the buttons with the given questions*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DialogSystem")
		void Show();

};

class UMGAsyncTask : public FNonAbandonableTask
{
	/*The subtitles that we're going to display*/
	TArray<FSubtitle> Subs;

	/*UI Reference*/
	UDIalogUI* DialogUI;

public:

	//Constructor
	UMGAsyncTask(TArray<FSubtitle>& Subs, UDIalogUI* DialogUI)
	{
		this->Subs = Subs;
		this->DialogUI = DialogUI;
	}

	/*Function needed by the UE in order to determine what's the tasks' status*/
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UMGAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

	/*This function executes each time this thread is active - UE4 searches for a function named DoWord() and executes it*/
	void DoWork()
	{
		
		float TimeToWait = 0.f;
		for (int32 i = 0; i < Subs.Num(); i++)
		{
			if (DialogUI)
			{
				//Sleep means that we pause this thread for the given time
			//FPlatformProcess::Sleep(Subs[i].AssociatedTime);
				FPlatformProcess::Sleep(Subs[i].AssociatedTime);


				//TimeToWait += Subs[i].AssociatedTime;
				//Update our subtitles after the thread comes back
				DialogUI->SubtitleToDisplay = Subs[i].Subtitle;
			}
			
		}
		
		//Sleep 1 second to let the user read the text
		//FPlatformProcess::Sleep(1.0f);
		
		//Clear the subtitle
		//DialogUI->SubtitleToDisplay = FString("");
	}
};