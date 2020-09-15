// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SAreaDoor.generated.h"

class UBoxComponent;

UCLASS()
class COOPGAME_API ASAreaDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASAreaDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Components")
	USceneComponent* Root;

	//hitbox for buying the door open
	UPROPERTY(EditAnywhere, Category = "Components")
	UBoxComponent* OpenBox;

	UPROPERTY(EditAnywhere, Category = "Components")
	UBoxComponent* CloseBox1;

	UPROPERTY(EditAnywhere, Category = "Components")
	UBoxComponent* CloseBox2;


	UPROPERTY(EditAnywhere, Category = "Components")
	float OpenSpeed;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	float CloseSpeed;

	bool bOpening;

	bool bClosing;

	bool bOpen;

	UPROPERTY(EditAnywhere, Category = "Components")
	float OpenOffset;

	UPROPERTY(EditAnywhere, Category = "Components")
	FVector OpenLocation;

	UPROPERTY(EditAnywhere, Category = "Components")
	FVector StartLocation;


	UFUNCTION()
	void OnOpenBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCloseBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Open();

};
