// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame/CoopGame.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "SCharacter.h"
#include "SNormalBullet.h"
#include "Misc/DateTime.h"
#include "Misc/Timespan.h"


static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	
	MuzzleSocketName = "Muzzle";
	TracerTargetName = "Target";
	BaseDamage = 20.0f;

	MaxBulletSpread = 1.0f;

	RateOfFire = 600.0f;
	MagSize = 30;
	ReloadTime = 1.0f;

}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	TimeBetweenShots = 60/RateOfFire;
	CurrentAmmo = MagSize;

	reloading = false;
}

void ASWeapon::Fire()
{
	
	if (CurrentAmmo > 0)
	{
		CurrentAmmo--;


		//trace a line from pawn eyes to crosshair location(center screen) to find impact point, then trace a line from the gun to impact point
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{


			//BULLET SPREAD
			//find out how long player has been firing their weapon for bullet spread
			FVector EyeLocation;
			FRotator EyeRotation;
			MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
			FVector ShotDirection = EyeRotation.Vector();
			FDateTime DT = FDateTime::Now();
			int32 CurrTimeMS = ((DT.GetHour() * 60 * 60) + (DT.GetMinute() * 60) + DT.GetSecond()) * 1000 + DT.GetMillisecond();
			int32 ContinousFireTime = CurrTimeMS - FireStartTime;
			float BulletSpread = (ContinousFireTime / BulletSpreadTimer) * MaxBulletSpread;
			BulletSpread = BulletSpread + MinBulletSpread;
			if (BulletSpread > MaxBulletSpread) 
			{
				BulletSpread = MaxBulletSpread;
			}
			float HalfRad = FMath::DegreesToRadians(BulletSpread);
			ShotDirection  = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);
			//UE_LOG(LogTemp, Warning, TEXT("Conitous Fire Time:  %d"), ContinousFireTime);
			

			//LINE TRACING
			//first line trace from camera to find impact point
			MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(MyOwner);
			QueryParams.AddIgnoredActor(this);
			QueryParams.bTraceComplex = true;
			QueryParams.bReturnPhysicalMaterial = true;
			FRotator ShootToRot;
			FVector TraceEnd = EyeLocation + (ShotDirection * 10000);
			FVector TracerEndPoint = TraceEnd;
			EPhysicalSurface SurfaceType = SurfaceType_Default;
			FHitResult Hit;
			if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
			{
				AActor* HitActor = Hit.GetActor();
				TracerEndPoint = Hit.ImpactPoint;
			    ShootToRot = UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, Hit.ImpactPoint);

			}
			else
			{
				ShootToRot = UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, TraceEnd);
			}


			//SPAWN BULLET PROJECTILE
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor* BulletActor = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, ShootToRot, SpawnParams);
			ASNormalBullet* Bullet = Cast<ASNormalBullet>(BulletActor);
			if (Bullet) {
				Bullet->SetOwner(this);
				Bullet->SetDamage(BaseDamage);
			}

			//PLAY EFFECTS AND RESET FIRE TIME
			UGameplayStatics::PlaySoundAtLocation(this, ShotSound, MuzzleLocation);
			PlayFireEffects(TracerEndPoint);
			LastFireTime = GetWorld()->TimeSeconds;


			//debug fire line
			if (DebugWeaponDrawing > 0)
			{
				DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);

			}
		}

	}
	else
	{
		//play dry fire sound and start reload
	    MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, MuzzleLocation);
		StartReload();
	}
	
}

float ASWeapon::GetBaseDamage()
{
	return BaseDamage;
}

void ASWeapon::SetBaseDamage(float NewDamage)
{
	BaseDamage = NewDamage;
}

void ASWeapon::StartFire()
{

	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);

}


void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}


void ASWeapon::StartReload()
{
	//don't reload if magazine already full
	if (CurrentAmmo == MagSize)
	{
		return;
	}
	//only reload if player has ammo
	AActor* MyOwner = GetOwner();
	ASCharacter* MyCharacter = Cast<ASCharacter>(MyOwner);

	if (MyCharacter->ARAmmo > 0)
	{
		//play sound
		//FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, MuzzleLocation);

		reloading = true;
		StopFire();
		GetWorldTimerManager().SetTimer(TimerHandle_ReloadTime, this, &ASWeapon::FinishReload, ReloadTime, false);
	}

}


void ASWeapon::FinishReload()
{
	reloading = false;

	AActor* MyOwner = GetOwner();
	ASCharacter* MyCharacter = Cast<ASCharacter>(MyOwner);

	float NeededAmmo = MagSize - CurrentAmmo;
	if (MyCharacter->ARAmmo >= NeededAmmo)
	{
		CurrentAmmo = MagSize;
		MyCharacter->ARAmmo = MyCharacter->ARAmmo - NeededAmmo;
	}
	else
	{
		CurrentAmmo = CurrentAmmo + MyCharacter->ARAmmo;
		MyCharacter->ARAmmo = 0;
	}
	
	GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);
}




void ASWeapon::PlayFireEffects(FVector TracerEnd)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);

	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}

}

