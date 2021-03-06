// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TDGame/Public/InventorySystem/TDInventoryComponent.h"
#include "TDPickUp.generated.h"

UCLASS(BlueprintType, Blueprintable)
class TDGAME_API UTDPickUp : public UObject
{//, EditInlineNew, DefaultToInstanced
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UTDPickUp();

	FORCEINLINE virtual class UWorld* GetWorld() const { return World; };
	
	UPROPERTY(Transient)
	class UWorld* World;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Pickup)
	FText UseActionText;

	/*The Texture of the item in case we want to add it in the secrets or inventory*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Pickup)
	class UTexture2D* PickupTexture;

	/*The name of the item*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Pickup)
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Pickup, meta = (Multiline = true))
	FText ItemDiscription;

	UPROPERTY()
		class UTDInventoryComponent* OwningInventory;

	virtual void Use(class ATDPaperCharacter* Character);

	UFUNCTION(BlueprintImplementableEvent)
		void OnUse(class ATDPaperCharacter* Character);

};
