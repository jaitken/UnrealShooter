// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SNormalBullet.generated.h"


class USphereComponent;
class ASWeapon;

UCLASS()
class COOPGAME_API ASNormalBullet : public AActor
{
	GENERATED_BODY()
	
protected:
	
	virtual void BeginPlay() override;

	// Sets default values for this actor's properties
	ASNormalBullet();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	//keep track of last damaged actor so a bullet cant damage an actor twice
	AActor* LastDamagedActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float TraceBeforeDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float TraceAfterDistance;

	AActor* Weapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint, FVector ShotDirection);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyHit(class UPrimitiveComponent * MyComp,
		AActor * Other,
		class UPrimitiveComponent * OtherComp,
		bool bSelfMoved,
		FVector HitLocation,
		FVector HitNormal,
		FVector NormalImpulse,
		const FHitResult & Hit) override;

};
