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

	/*
	UPROPERTY(EditInstanceOnly, Category = "SpawnPoints")
	AActor* SpawnPoint1;

	UPROPERTY(EditInstanceOnly, Category = "SpawnPoints")
	AActor* SpawnPoint2;

	UPROPERTY(EditInstanceOnly, Category = "SpawnPoints")
	AActor* SpawnPoint3;

	UPROPERTY(EditInstanceOnly, Category = "SpawnPoints")
	AActor* SpawnPoint4;

	UPROPERTY(EditInstanceOnly, Category = "SpawnPoints")
	AActor* SpawnPoint5;

	UPROPERTY(EditInstanceOnly, Category = "SpawnPoints")
	AActor* SpawnPoint6;

	UPROPERTY(EditInstanceOnly, Category = "SpawnPoints")
	AActor* SpawnPoint7;
	*/

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Open();

	UPROPERTY(EditAnywhere, Category = "Door")
	float PriceToOpen;

};
