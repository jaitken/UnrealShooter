// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMotherBot.generated.h"

class USHealthComponent;

UCLASS()
class COOPGAME_API ASMotherBot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASMotherBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComp;

	FVector GetNextPathPoint();

	//next point in nav path
	FVector NextPathPoint;

	void UpdatePath();

	FTimerHandle TimerHandle_FindNextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "MotherBot")
	float MovementForce;

	bool bExploded;

	UPROPERTY(EditDefaultsOnly, Category = "MotherBot")
	bool bUseVelocityChange;

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
							class AController* InstigatedBy, AActor* DamageCauser);

	FTimerHandle TimerHandle_SpawnBot;

	UPROPERTY(EditDefaultsOnly, Category = "MotherBot")
	float TimeToSpawnBot;

	void SpawnBot();

	UFUNCTION(BlueprintImplementableEvent, Category = "MotherBot")
	void BlueprintSpawnBot();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
