// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeaponPickup.generated.h"

class ASWeapon;
class USphereComponent;

UCLASS()
class COOPGAME_API ASWeaponPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeaponPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ASWeapon* WeaponInstance;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(EditInstanceOnly, Category = "WeaponPickup")
	float CooldownDuration;

	FTimerHandle TimerHandle_WeaponRespawnTimer;

	void Respawn();

	bool bWeaponReady;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditInstanceOnly, Category = "WeaponPickup")
	TSubclassOf<ASWeapon> WeaponClass;

};
