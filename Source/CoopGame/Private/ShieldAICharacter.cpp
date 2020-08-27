// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldAICharacter.h"

AShieldAICharacter::AShieldAICharacter()
{
	UE_LOG(LogTemp, Warning, TEXT("Shield AI constructer"))
		SheildAttachSocketName = "ShieldSocket";
}

void AShieldAICharacter::BeginPlay()
{

	Super::BeginPlay();
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Shield = GetWorld()->SpawnActor<AActor>(StarterShield, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (Shield)
	{
		Shield->SetActorHiddenInGame(false);
		Shield->SetOwner(this);
		Shield->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SheildAttachSocketName);
	}
}
