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
	for (auto& Spawn : SpawnPoints)
	{
		Spawn->Tags.Add(FName("ActiveSpawn"));
	}

	Destroy();
}
