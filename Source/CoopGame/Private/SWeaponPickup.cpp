// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeaponPickup.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "SWeapon.h"
#include "SCharacter.h"

// Sets default values
ASWeaponPickup::ASWeaponPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;

	CooldownDuration = 10.0f;

	bWeaponReady = true;

}

// Called when the game starts or when spawned
void ASWeaponPickup::BeginPlay()
{
	Super::BeginPlay();

	Respawn();
	
}

void ASWeaponPickup::Respawn()
{
	if (WeaponClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon is nullptr in %s. please update blueprint"), *GetName());
		return;
	}

	bWeaponReady = true;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	WeaponInstance = GetWorld()->SpawnActor<ASWeapon>(WeaponClass, GetTransform(), SpawnParams);
}

// Called every frame
void ASWeaponPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


