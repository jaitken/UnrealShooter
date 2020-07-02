// Fill out your copyright notice in the Description page of Project Settings.


#include "SWall.h"
#include "Components/BoxComponent.h"

// Sets default values
ASWall::ASWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ASWall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASWall::Open()
{
	if (SpawnPoint1) 
	{
		SpawnPoint1->Tags.Add(FName("ActiveSpawn"));
	}
	if (SpawnPoint2)
	{
		SpawnPoint2->Tags.Add(FName("ActiveSpawn"));
	}
	if (SpawnPoint3)
	{
		SpawnPoint3->Tags.Add(FName("ActiveSpawn"));
	}
	if (SpawnPoint4)
	{
		SpawnPoint4->Tags.Add(FName("ActiveSpawn"));
	}
	if (SpawnPoint5)
	{
		SpawnPoint5->Tags.Add(FName("ActiveSpawn"));
	}
	if (SpawnPoint6)
	{
		SpawnPoint6->Tags.Add(FName("ActiveSpawn"));
	}
	if (SpawnPoint7)
	{
		SpawnPoint7->Tags.Add(FName("ActiveSpawn"));
	}
	Destroy();
}
