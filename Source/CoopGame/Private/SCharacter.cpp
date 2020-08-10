// Fill out your copyright notice in the Description page of Project Settings.
#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SWeapon.h"
#include "Components/CapsuleComponent.h"
#include "CoopGame/CoopGame.h"
#include "Components/SHealthComponent.h"
#include "SWeaponPickup.h"
#include "SBeamRifle.h"
#include "SWall.h"
#include "AmmoPickup.h"
#include "SDBShotgun.h"
#include "Misc/DateTime.h"
#include "Misc/Timespan.h"


// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	
	//only want mesh to respond to collision, not capsule
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	SniperFOV = 40.0f;
	ZoomedFOV = 65.0f;
	DefZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20;

	WeaponAttachSocketName = "WeaponSocket";
	WeaponBackSocketName = "WeaponBackSocket";

	ARAmmo = 240;
	ShotgunAmmo = 10;
	SniperAmmo = 10;
	LightAmmo = 300;
}


// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;

	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

		//spawn default weapon 
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		Weapon1 = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (Weapon1)
		{
			Weapon1->SetOwner(this);
			Weapon1->SetActorHiddenInGame(true);
		}
		Weapon2 = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass2, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (Weapon2)
		{
			Weapon2->SetOwner(this);
			Weapon2->SetActorHiddenInGame(true);
		}
		Weapon3 = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass3, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (Weapon3)
		{
			Weapon3->SetOwner(this);
			Weapon3->SetActorHiddenInGame(true);
		}
		

		CurrentWeapon = Weapon1;
		if (CurrentWeapon)
		{
			CurrentWeapon->SetActorHiddenInGame(false);
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
	
	
}


void ASCharacter::MoveForward(float Value)
{

	AddMovementInput(GetActorForwardVector()*Value);
}


void ASCharacter::MoveRight(float Value)
{

	AddMovementInput(GetActorRightVector()*Value);
}


void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}



void ASCharacter::BeginZoom()
{
	bWantsToZoom = true;
	ASBeamRifle* temp = Cast<ASBeamRifle>(CurrentWeapon);
	if(temp)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Beam rifle Zoom"));
		ZoomedFOV = SniperFOV;
		
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("other Zoom"));
		ZoomedFOV = DefZoomedFOV;

	}
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ASCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		//if weapon is double barrel shotgun ignore reloading restriction
		/*
		ASDBShotgun* temp = Cast<ASDBShotgun>(CurrentWeapon);
		if (temp)
		{
			UE_LOG(LogTemp, Warning, TEXT("DB Shotgun in hand"))
			CurrentWeapon->StartFire();
			return;
		}
		*/
		FDateTime DT = FDateTime::Now();
		int32 CurrentTimeH = DT.GetHour();
		int32 CurrentTimeM = DT.GetMinute();
		int32 CurrentTimeS = DT.GetSecond();
		int32 CurrentTimeMi = DT.GetMillisecond();
		StartTime = ((DT.GetHour() * 60 * 60) + (DT.GetMinute() * 60) + DT.GetSecond())*1000 + DT.GetMillisecond();
		CurrentWeapon->FireStartTime = StartTime;
		//UE_LOG(LogTemp, Log, TEXT("%d : %d : %d : %d"), CurrentTimeH, CurrentTimeM, CurrentTimeS, CurrentTimeMi);
		
		
		
		if (!CurrentWeapon->reloading)
		{
			CurrentWeapon->StartFire();

		}
	}
}

void ASCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		FDateTime DT = FDateTime::Now();
		int32 EndTimeH = DT.GetHour();
		int32 EndTimeM = DT.GetMinute();
		int32 EndTimeS = DT.GetSecond();
		int32 EndTimeMi = DT.GetMillisecond();
		int32 CurrTimeMS = ((DT.GetHour() * 60 * 60) + (DT.GetMinute() * 60) + DT.GetSecond())*1000 + DT.GetMillisecond();
		int32 TimeBetween = CurrTimeMS - StartTime;
		//UE_LOG(LogTemp, Log, TEXT("%d : %d : %d : %d"), EndTimeH, EndTimeM, EndTimeS, EndTimeMi);
		//UE_LOG(LogTemp, Warning, TEXT("TIMEBETWEEN  %d"), TimeBetween);
		CurrentWeapon->StopFire();
		CurrentWeapon->FireStartTime = 0;
	}
}

void ASCharacter::Reload()
{
	if (CurrentWeapon)
	{
		if (!CurrentWeapon->reloading)
		{
			CurrentWeapon->StartReload();

		}
	}
}

void ASCharacter::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied)
	{
		bDied = true;
		//death
		GetMovementComponent()->StopMovementImmediately();

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(10.0f);

		CurrentWeapon->StopFire();
		CurrentWeapon->SetLifeSpan(10.0f);

		//enemies drop ammo/cash on death
		SpawnAmmo();

	}

}

void ASCharacter::SwitchWeapon()
{
	/*
		if (BackWeapon)
		{
			ASWeapon* WeaponOnBack = BackWeapon;
			ASWeapon* WeaponInHand = CurrentWeapon;

			//UE_LOG(LogTemp, Warning, TEXT("Weapon Switch Attemped"))
			//put current weapon on back and assign it to Backweapon
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponBackSocketName);
			BackWeapon = WeaponInHand;

			WeaponOnBack->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
			CurrentWeapon = WeaponOnBack;

		}
		*/
	
}

void ASCharacter::SwitchToWeapon1()
{
	//do nothing if weapon1 is out
	if (CurrentWeapon == Weapon1)
	{
		return;
	}
	CurrentWeapon->SetActorHiddenInGame(true);
	CurrentWeapon = Weapon1;
	CurrentWeapon->SetActorHiddenInGame(false);
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);

}

void ASCharacter::SwitchToWeapon2()
{
	//do nothing if weapon2 is out
	if (CurrentWeapon == Weapon2)
	{
		return;
	}
	CurrentWeapon->SetActorHiddenInGame(true);
	CurrentWeapon = Weapon2;
	CurrentWeapon->SetActorHiddenInGame(false);
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
}

void ASCharacter::SwitchToWeapon3()
{
	//do nothing if weapon3 is out
	if (CurrentWeapon == Weapon3)
	{
		return;
	}
	CurrentWeapon->SetActorHiddenInGame(true);
	CurrentWeapon = Weapon3;
	CurrentWeapon->SetActorHiddenInGame(false);
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
}

void ASCharacter::PickUpWeapon()
{
	if (bPlayerOverWeapon) {
		
		if (Money >= WeaponPickupCost) 
		{
			Money = Money - WeaponPickupCost;
			SetWeapon(WeaponPickUpClass);
			
		}

	}

}

void ASCharacter::BuyDoor()
{
	//UE_LOG(LogTemp, Warning, TEXT("BUY Door Attempted"))
	if (bPlayerNearDoor)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Door Destoryed"))

			if (Money >= DoorCost) 
			{
				Money = Money - DoorCost;
				DoorToOpen->Open();
			}
	}

}

void ASCharacter::AddMoney()
{
	Money = Money + 100;
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);

}


// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);	
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::Reload);

	PlayerInputComponent->BindAction("SwitchWeapon", IE_Pressed, this, &ASCharacter::SwitchWeapon);
	PlayerInputComponent->BindAction("PickUpWeapon", IE_Pressed, this, &ASCharacter::PickUpWeapon);

	PlayerInputComponent->BindAction("SwitchWeapon", IE_Pressed, this, &ASCharacter::SwitchWeapon);
	
	PlayerInputComponent->BindAction("SwitchToWeapon1", IE_Pressed, this, &ASCharacter::SwitchToWeapon1);
	PlayerInputComponent->BindAction("SwitchToWeapon2", IE_Pressed, this, &ASCharacter::SwitchToWeapon2);
	PlayerInputComponent->BindAction("SwitchToWeapon3", IE_Pressed, this, &ASCharacter::SwitchToWeapon3);

	PlayerInputComponent->BindAction("BuyDoor", IE_Pressed, this, &ASCharacter::BuyDoor);

	//for testing money
	PlayerInputComponent->BindAction("AddMoney", IE_Pressed, this, &ASCharacter::AddMoney);


}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

ASWeapon * ASCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

ASWeapon * ASCharacter::GetWeapon1()
{
	return Weapon1;
}

ASWeapon * ASCharacter::GetWeapon2()
{
	return Weapon2;
}

ASWeapon * ASCharacter::GetWeapon3()
{
	return Weapon3;
}

ASWeapon * ASCharacter::GetBackWeapon()
{
	return BackWeapon;
}

void ASCharacter::SetWeapon(TSubclassOf<ASWeapon> NewWeaponClass)
{
	
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;		
		
		if (CurrentWeapon)
		{
			ASWeapon* NewWeapon = GetWorld()->SpawnActor<ASWeapon>(NewWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

			//determine which type of weapon it is by checking actor tags
			if (NewWeapon->ActorHasTag(TEXT("Shotgun")))
			{
				Weapon2->Destroy();
				Weapon2 = NewWeapon;
				Weapon2->SetOwner(this);
				Weapon2->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
				Weapon2->SetActorHiddenInGame(true);
				//CurrentWeapon = Weapon2;
			}
			//CurrentWeapon->Destroy();
		}

		//CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(NewWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		/*
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
		*/

}


void ASCharacter::NotifyActorBeginOverlap(AActor * OtherActor)
{
	//for weapon pickups
	ASWeaponPickup* temp = Cast<ASWeaponPickup>(OtherActor);
	if (temp) {
		WeaponPickupCost = temp->WeaponPrice;
		WeaponPickUpClass = temp->WeaponClass;
		bPlayerOverWeapon = true;

		AmmoTypeToAdd = temp->AmmoType;

	}

	//for opening doors
	ASWall* Wall = Cast<ASWall>(OtherActor);
	if(Wall)
	{
		bPlayerNearDoor = true;
		DoorCost = Wall->PriceToOpen;
		DoorToOpen = Wall;

	}

}

void ASCharacter::NotifyActorEndOverlap(AActor * OtherActor)
{
	//for weapon pickups
	ASWeaponPickup* temp = Cast<ASWeaponPickup>(OtherActor);
	if (temp) {
		bPlayerOverWeapon = false;
	}

	//for doors
	ASWall* Wall = Cast<ASWall>(OtherActor);
	if (Wall)
	{
		bPlayerNearDoor = false;

	}

}
