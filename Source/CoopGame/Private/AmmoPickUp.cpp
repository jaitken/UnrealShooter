// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickUp.h"
#include "SCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"




// Sets default values
AAmmoPickUp::AAmmoPickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(300.0f);
	SphereComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAmmoPickUp::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAmmoPickUp::AddAmmo(ASCharacter * Player)
{
	if (AmmoType == EAmmoType::ARAmmo)
	{
		Player->ARAmmo += AmmoAmount;
	}

	if (AmmoType == EAmmoType::ShotgunAmmo)
	{
		Player->ShotgunAmmo += AmmoAmount;
	}

	if (AmmoType == EAmmoType::SniperAmmo)
	{
		Player->SniperAmmo += AmmoAmount;
	}

	Destroy();
}

// Called every frame
void AAmmoPickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAmmoPickUp::NotifyActorBeginOverlap(AActor * OtherActor)
{
	ASCharacter* MyCharacter = Cast<ASCharacter>(OtherActor);
	if (MyCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ammo Picked up"))
		AddAmmo(MyCharacter);
	}
}

