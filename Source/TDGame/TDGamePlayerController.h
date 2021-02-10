// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TDGamePlayerController.generated.h"

UCLASS()
class ATDGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATDGamePlayerController();

protected:

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;

	virtual void SetupInputComponent() override;
	
protected:

	virtual void BeginPlay() override;

public:
	
	
};


