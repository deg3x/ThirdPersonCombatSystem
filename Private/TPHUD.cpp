// Fill out your copyright notice in the Description page of Project Settings.

#include "TPHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

ATPHUD::ATPHUD()
{
	/*static ConstructorHelpers::FObjectFinder<UUserWidget> PrimaryUI(TEXT("/Game/UI/WB_PlayerHealth.WB_PlayerHealth"));

	PlayerPrimaryUI = PrimaryUI.Object;*/
}

void ATPHUD::BeginPlay()
{
	/*if (PlayerPrimaryUI == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[!] Primary UI blueprint not found"));

		return;
	}

	PlayerPrimaryUI->*/
}

void ATPHUD::DrawHUD()
{
	Super::DrawHUD();
}

