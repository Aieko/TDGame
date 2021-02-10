// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TDGamePlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Engine/World.h"
#include "TDGame/Public/TDPaperCharacter.h"

ATDGamePlayerController::ATDGamePlayerController()
{
	
}

void ATDGamePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

}

void ATDGamePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	
}

void ATDGamePlayerController::BeginPlay()
{
	Super::BeginPlay();


}




