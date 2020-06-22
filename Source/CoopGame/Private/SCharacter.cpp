// Fill out your copyright notice in the Description page of Project Settings.
#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SWeapon.h"
#include "Components/CapsuleComponent.h"
#include "CoopGame/CoopGame.h"
#include "..\Public\SCharacter.h"
#include "Components/SHealthComponent.h"
#include "SWeaponPickup.h"


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

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20;

	WeaponAttachSocketName = "WeaponSocket";
	WeaponBackSocketName = "WeaponBackSocket";

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

		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (CurrentWeapon)
		{
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
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ASCharacter::StartFire()
{
	if (CurrentWeapon)
	{
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
		CurrentWeapon->StopFire();
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

	}

}

void ASCharacter::SwitchWeapon()
{

		if (BackWeapon)
		{
			ASWeapon* WeaponOnBack = BackWeapon;
			ASWeapon* WeaponInHand = CurrentWeapon;

			UE_LOG(LogTemp, Warning, TEXT("Weapon Switch Attemped"))
			//put current weapon on back and assign it to Backweapon
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponBackSocketName);
			BackWeapon = WeaponInHand;

			WeaponOnBack->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
			CurrentWeapon = WeaponOnBack;

		}
	
}

void ASCharacter::PickUpWeapon()
{
	if (bPlayerOverWeapon) {

		SetWeapon(WeaponPickUpClass);

	}

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

void ASCharacter::SetWeapon(TSubclassOf<ASWeapon> NewWeaponClass)
{
	
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//if no weapon is on back weapon you just picked up goes on back
		if (!BackWeapon) 
		{
			BackWeapon = GetWorld()->SpawnActor<ASWeapon>(NewWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			BackWeapon->SetOwner(this);
			BackWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponBackSocketName);
			return;
		}
		
		
		if (CurrentWeapon)
		{
			CurrentWeapon->Destroy();
		}

		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(NewWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
	

}


void ASCharacter::NotifyActorBeginOverlap(AActor * OtherActor)
{
	ASWeaponPickup* temp = Cast<ASWeaponPickup>(OtherActor);
	if (temp) {
		WeaponPickUpClass = temp->WeaponClass;
		bPlayerOverWeapon = true;
	}
}

void ASCharacter::NotifyActorEndOverlap(AActor * OtherActor)
{
	ASWeaponPickup* temp = Cast<ASWeaponPickup>(OtherActor);
	if (temp) {
		bPlayerOverWeapon = false;
	}

}
