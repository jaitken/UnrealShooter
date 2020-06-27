// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmmoPickUp.generated.h"

class USphereComponent;
class ASCharacter;



UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	ARAmmo,

	ShotgunAmmo,

	SniperAmmo


};


UCLASS()
class COOPGAME_API AAmmoPickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmoPickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	EAmmoType AmmoType;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	float AmmoAmount;

	void AddAmmo(ASCharacter* Player);



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
