// Fill out your copyright notice in the Description page of Project Settings.


#include "TDGame/Public/DIalogUI.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void UDIalogUI::UpdateSubtitles(TArray<FSubtitle> Subtitles)
{
	if (!Subtitles.IsValidIndex(0)) return;
	
	//Start a background task in a low priority thread
	(new FAutoDeleteAsyncTask<UMGAsyncTask>(Subtitles, this))->StartBackgroundTask();
}

void UMGAsyncTask::DoWork()
{

	
	for (int32 i = 0; i < Subs.Num(); i++)
	{
		if (DialogUI)
		{
			//Sleep means that we pause this thread for the given time
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

