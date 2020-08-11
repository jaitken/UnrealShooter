// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupPickup.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "SPowerupActor.h"
#include "SCharacter.h"

// Sets default values
ASPowerupPickup::ASPowerupPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PowerupInterval = 0.0f;
	TotalNumOfTciks = 0;

	bIsPowerupActive = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	SphereComp->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void ASPowerupPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASPowerupPickup::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerupTicked();

	if (TicksProcessed >= TotalNumOfTciks)
	{
		OnExpired();
		bIsPowerupActive = false;
		OnPowerupStateChanged(bIsPowerupActive);

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
		Destroy();
	}
}

void ASPowerupPickup::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	//check to see if overlapped actor is a player
	ASCharacter* tempChar = Cast<ASCharacter>(OtherActor);
	if (tempChar && !bIsPowerupActive && tempChar->bIsPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character Overlapped"));
		ActivatePowerup(OtherActor);
	}

}

void ASPowerupPickup::ActivatePowerup(AActor * ActiveFor)
{
	OnActivated(ActiveFor);

	bIsPowerupActive = true;
	MeshComp->SetVisibility(false);
	OnPowerupStateChanged(bIsPowerupActive);

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupPickup::OnTickPowerup, PowerupInterval, true);
	}
	else
	{
		OnTickPowerup();
	}
}

