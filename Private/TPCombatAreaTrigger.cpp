// Fill out your copyright notice in the Description page of Project Settings.

#include "TPCombatAreaTrigger.h"
#include "Components/BoxComponent.h"
#include "TPPlayerCharacter.h"
#include "TPEnemyBase.h"
#include "TPGameModeBase.h"

ATPCombatAreaTrigger::ATPCombatAreaTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RootComponent = BoxComponent;

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ATPCombatAreaTrigger::HandleOverlap);
}

void ATPCombatAreaTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATPCombatAreaTrigger::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
										bool bFromSweep, const FHitResult& SweepResult)
{
	ATPPlayerCharacter* Player = Cast<ATPPlayerCharacter>(OtherActor);

	if (Player == nullptr)
	{
		return;
	}

	if (BoundEnemy == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[!] Bound enemy to trigger not assigned!"));
		return;
	}

	ATPGameModeBase* GameMode = Cast<ATPGameModeBase>(GetWorld()->GetAuthGameMode());

	if (GameMode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[!] Problem finding game mode..."));
		return;
	}

	Player->SetCurrentEnemyBoss(BoundEnemy);
	Player->SetCombatCameraEnabled(true);

	GameMode->InitiateBossFight(BoundEnemy);

	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}
