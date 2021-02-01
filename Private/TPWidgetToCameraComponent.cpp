// Fill out your copyright notice in the Description page of Project Settings.

#include "TPWidgetToCameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TPEnemyHealthWidget.h"

UTPWidgetToCameraComponent::UTPWidgetToCameraComponent()
{

}

void UTPWidgetToCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	HealthWidget = Cast<UTPEnemyHealthWidget>(GetUserWidgetObject());
	if (HealthWidget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[!] Could not obtain health widget!"));
	}
}

void UTPWidgetToCameraComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FacePlayerCamera();
}

void UTPWidgetToCameraComponent::FacePlayerCamera()
{
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	if (CameraManager == nullptr)
	{
		return;
	}

	FVector CameraLocation = CameraManager->GetCameraLocation();
	FVector WidgetLocation = GetComponentLocation();
	
	FRotator NewRotation = GetRelativeRotation();
	
	NewRotation = UKismetMathLibrary::FindLookAtRotation(WidgetLocation, CameraLocation);

	SetWorldRotation(NewRotation);
}

void UTPWidgetToCameraComponent::UpdateHealthWidgetCurrentHealth(float CurrentHealth)
{
	HealthWidget->UpdateCurrentHealth(CurrentHealth);
}

void UTPWidgetToCameraComponent::SetHealthWidgetValues(float MaxHealth, float CurrentHealth)
{
	HealthWidget->SetMaxHealth(MaxHealth);
	HealthWidget->SetCurrentHealth(CurrentHealth);
}

void UTPWidgetToCameraComponent::SetEnemyOwner(ATPEnemyBase* Owner)
{
	HealthWidget->SetEnemyOwner(Owner);
}