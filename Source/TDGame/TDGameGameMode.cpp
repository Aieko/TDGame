// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TDGameGameMode.h"
#include "TDGamePlayerController.h"
#include "UObject/ConstructorHelpers.h"

ATDGameGameMode::ATDGameGameMode()
{
	/*// use our custom PlayerController class
	PlayerControllerClass = ATDGamePlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}*/
}