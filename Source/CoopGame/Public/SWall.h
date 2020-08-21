// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWall.generated.h"

class UBoxComponent;

UCLASS()
class COOPGAME_API ASWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//actual blocking door
	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	//hitbox for buying the door open
	UPROPERTY(EditAnywhere, Category = "Components")
	UBoxComponent* BoxComp;

public:
	UPROPERTY(EditInstanceOnly, Category = "SpawnPoints")
	TArray<AActor*> SpawnPoints;

	//other doors to destroy when this door is bought
	UPROPERTY(EditInstanceOnly, Category = "LinkedDoors")
	TArray<AActor*> LinkedDoors;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Open();

	UPROPERTY(EditAnywhere, Category = "Door")
	float PriceToOpen;

};
