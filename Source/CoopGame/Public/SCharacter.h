// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoPickup.h"
#include "SCharacter.generated.h"

class USHealthComponent;
class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class ASWall;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ASWeapon* Weapon1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ASWeapon* Weapon2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ASWeapon* Weapon3;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ASWeapon* Weapon4;


	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass2;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass3;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass4;

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

	void SwitchToWeapon1();

	void SwitchToWeapon2();

	void SwitchToWeapon3();

	void SwitchToWeapon4();

	void PickUpWeapon();

	TSubclassOf<ASWeapon> WeaponPickUpClass;

	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void SpawnAmmo();


	//Doors
	void BuyDoor();

	bool bPlayerNearDoor;

	float DoorCost;

	ASWall* DoorToOpen;

	//add money to character for testing
	void AddMoney();

public:	

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	bool bIsPlayer;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;


	//used for to find ammo count for HUD
	UFUNCTION(BlueprintCallable, Category = "Components")
	ASWeapon* GetCurrentWeapon();

	UFUNCTION(BlueprintCallable, Category = "Components")
	ASWeapon* GetWeapon1();

	UFUNCTION(BlueprintCallable, Category = "Components")
	ASWeapon* GetWeapon2();

	UFUNCTION(BlueprintCallable, Category = "Components")
	ASWeapon* GetWeapon3();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player")
	float LightAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player")
	float Money;

	float WeaponPickupCost;

	float AmmoToAdd;

	EAmmoType AmmoTypeToAdd;


	//shottimer
	int32 StartTime;

};
