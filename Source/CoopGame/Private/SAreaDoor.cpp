// Fill out your copyright notice in the Description page of Project Settings.


#include "SAreaDoor.h"
#include "Components/BoxComponent.h"
#include "SCharacter.h"
#include "Components/SceneComponent.h"

// Sets default values
ASAreaDoor::ASAreaDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = Root;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));

	OpenBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OpenBox"));
	OpenBox->SetupAttachment(RootComponent);

	OpenBox->OnComponentBeginOverlap.AddDynamic(this, &ASAreaDoor::OnOpenBoxBeginOverlap);

	CloseBox1 = CreateDefaultSubobject<UBoxComponent>(TEXT("CloseBox1"));
	CloseBox1->SetupAttachment(RootComponent);

	CloseBox2 = CreateDefaultSubobject<UBoxComponent>(TEXT("CloseBox2"));
	CloseBox2->SetupAttachment(RootComponent);

	CloseBox1->OnComponentBeginOverlap.AddDynamic(this, &ASAreaDoor::OnCloseBoxBeginOverlap);
	CloseBox2->OnComponentBeginOverlap.AddDynamic(this, &ASAreaDoor::OnCloseBoxBeginOverlap);
}

// Called when the game starts or when spawned
void ASAreaDoor::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	OpenLocation = StartLocation + FVector(0, 0, OpenOffset);
	
}

void ASAreaDoor::OnOpenBoxBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	/*
	ASCharacter* temp = Cast<ASCharacter>(OtherActor);
	if (temp && !bOpening) {
		UE_LOG(LogTemp, Warning, TEXT("OPEN BOX OVERLAPPED"));
		bOpening = true;
		//MeshComp->SetVisibility(false);
		//MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	}
	*/
}

void ASAreaDoor::OnCloseBoxBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
	ASCharacter* temp = Cast<ASCharacter>(OtherActor);
	if (temp) {
		bClosing = true;
		UE_LOG(LogTemp, Warning, TEXT("CLOSE BOX OVERLAPPED"));
		//MeshComp->SetVisibility(true);
		//MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	
}

// Called every frame
void ASAreaDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bOpening) 
	{
		bOpen = true;
		FVector CurrentLocation = MeshComp->GetComponentLocation();
		CurrentLocation += GetActorUpVector()*OpenSpeed*DeltaTime;
		MeshComp->SetWorldLocation(CurrentLocation);

		if (CurrentLocation.Z >= StartLocation.Z+OpenOffset)
		{
			bOpening = false;
		}
	}

	if (bClosing && bOpen)
	{
		bOpening = false;
		FVector CurrentLocation = MeshComp->GetComponentLocation();
		CurrentLocation += GetActorUpVector()*-1*CloseSpeed*DeltaTime;
		//SetActorLocation(CurrentLocation);
		MeshComp->SetWorldLocation(CurrentLocation);

		if (CurrentLocation.Z <= StartLocation.Z)
		{
			bClosing = false;
			bOpen = false;
		}
	}


}

void ASAreaDoor::Open()
{
	if (!bOpening) 
	{
		bOpening = true;
	}
}

