// Fill out your copyright notice in the Description page of Project Settings.


#include "TDGame/Public/TDHealthComponent.h"
#include "TDGame/Public/TDBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UTDHealthComponent::UTDHealthComponent()
{
	DefaultHealth = 100;
	bIsDead = false;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UTDHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UTDHealthComponent::HandleTakeAnyDamage);
		}
	
	Health = DefaultHealth;
	
}

void UTDHealthComponent::OnRep_Health(int32 OldHealth)
{
	int32 Damage = Health - OldHealth;
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void UTDHealthComponent::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{	
	if (Damage <= 0 || bIsDead)
	{
		return;
	}

	Health = FMath::Clamp(int(Health - Damage), 0, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	bIsDead = Health <= 0;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

int32 UTDHealthComponent::GetHealth()
{
	return Health;
}

void UTDHealthComponent::Heal(int32 Ammount)
{
	if (Health < 6)
	{
		Health += Ammount;
	}
	
	OnHealthChanged.Broadcast(this, Health, Ammount, nullptr, nullptr, nullptr);
}

// Called every frame
void UTDHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTDHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTDHealthComponent, Health);

}