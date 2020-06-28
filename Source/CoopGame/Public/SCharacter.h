// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class USHealthComponent;
class UCameraComponent;
class USpringArmComponent;
class ASWeapon;

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

	bool bWantsToZoom; 

	UPROPERTY(EditAnywhere, Category = "Player")
	float DefZoomedFOV;

	UPROPERTY(EditAnywhere, Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditAnywhere, Category = "Player")
	float SniperFOV;

	float DefaultFOV;

	void BeginZoom();

	void EndZoom();

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ASWeapon* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ASWeapon* BackWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponBackSocketName;

	void Reload();
	
	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bDied;

	void SwitchWeapon();

	void PickUpWeapon();

	TSubclassOf<ASWeapon> WeaponPickUpClass;

	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void SpawnAmmo();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;


	//used for to find ammo count for HUD
	UFUNCTION(BlueprintCallable, Category = "Components")
	ASWeapon* GetCurrentWeapon();

	UFUNCTION(BlueprintCallable, Category = "Components")
	ASWeapon* GetBackWeapon();

	void SetWeapon(TSubclassOf<ASWeapon> NewWeaponClass);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bPlayerOverWeapon;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player")
	float ARAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player")
	float ShotgunAmmo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player")
	float SniperAmmo;

};
