// Fill out your copyright notice in the Description page of Project Settings.

#include "TPPHealthComponent.h"
#include "GameFramework/Actor.h"

UTPPHealthComponent::UTPPHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTPPHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* ComponentOwner = GetOwner();
	if (ComponentOwner)
	{
		ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &UTPPHealthComponent::TakeDamage);
	}

	CurrentHealth = MaxHealth;
	
}

void UTPPHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
}

