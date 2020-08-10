// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupPickup.generated.h"

class USphereComponent;

UCLASS()
class COOPGAME_API ASPowerupPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//time between number of ticks
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float PowerupInterval;

	//total times we apply powerup effect
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 TotalNumOfTciks;

	UFUNCTION()
	void OnTickPowerup();

	FTimerHandle TimerHandle_PowerupTick;

	int32 TicksProcessed;

	UPROPERTY()
	bool bIsPowerupActive;

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupStateChanged(bool bNewIsActive);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

public:	

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void ActivatePowerup(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnActivated(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();

};
