// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame/CoopGame.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "SCharacter.h"
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
		UE_LOG(LogTemp, Log, TEXT("Shot Attempted"));
		//Play Sound 
		UGameplayStatics::PlaySoundAtLocation(this, ShotSound, MuzzleLocation);

		//trace a line from pawn eyes to crosshair location(center screen) to find impact point, then trace a line from the gun to impact point
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(MyOwner);
			QueryParams.AddIgnoredActor(this);
			QueryParams.bTraceComplex = true;
			QueryParams.bReturnPhysicalMaterial = true;

			FVector EyeLocation;
			FRotator EyeRotation;
			MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
			FVector ShotDirection = EyeRotation.Vector();

			//find out how long player has been firing their weapon  for bullet spread
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
			

			//first line trace from camera to find impact point
			FVector TraceEnd = EyeLocation + (ShotDirection * 10000);
			FVector TracerEndPoint = TraceEnd;
			EPhysicalSurface SurfaceType = SurfaceType_Default;
			FHitResult Hit;
			if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
			{
				//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Blue, false, 1.0f, 0, 1.0f);
				AActor* HitActor = Hit.GetActor();
				TracerEndPoint = Hit.ImpactPoint;
				DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.0f, 12, FColor::Red, false, 3, 0, 1 );

				FHitResult RealHit;
				MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
				DrawDebugSphere(GetWorld(), MuzzleLocation, 20.0f, 12, FColor::Green, false, 3, 0, 1);
				if (GetWorld()->LineTraceSingleByChannel(RealHit, MuzzleLocation, Hit.ImpactPoint, COLLISION_WEAPON, QueryParams))
				{
					UE_LOG(LogTemp, Warning, TEXT("RealHit found"));
					DrawDebugSphere(GetWorld(), RealHit.ImpactPoint, 20.0f, 12, FColor::Blue, false, 3, 0, 1);
				}
			}

			//second line trace from weapon to impact point
			/*
			FHitResult Hit1;
			MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
			if (GetWorld()->LineTraceSingleByChannel(Hit1, MuzzleLocation, TracerEndPoint, COLLISION_WEAPON, QueryParams))
			{
				//DrawDebugLine(GetWorld(), MuzzleLocation, TracerEndPoint, FColor::Red, false, 3.0f, 0, 1.0f);
				//Blocking hit, process damage
				AActor* HitActor = Hit1.GetActor();
				DrawDebugSphere(GetWorld(), Hit1.ImpactPoint, 20.0f, 12, FColor::Blue, false, 3, 0, 1);

				//determine surface type
				SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit1.PhysMaterial.Get());

				float ActualDamage = BaseDamage;
				if (SurfaceType == SURFACE_FLESHVULNERABLE)
				{
					ActualDamage *= 4.0f;
				}

				UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit1, MyOwner->GetInstigatorController(), MyOwner, DamageType);
				PlayImpactEffects(SurfaceType, Hit1.ImpactPoint);
				TracerEndPoint = Hit1.ImpactPoint;
			}
			*/
			PlayFireEffects(TracerEndPoint);

			LastFireTime = GetWorld()->TimeSeconds;
			CurrentAmmo--;

			//debug fire line
			if (DebugWeaponDrawing > 0)
			{
				DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);

			}
			//DrawDebugLine(GetWorld(), MuzzleLocation, TraceEnd, FColor::Red, false, 3.0f, 0, 1.0f);
		}

	}
	else
	{
		//play dry fire sound and start reload
		//FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
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


	if (TracerEffect)
	{

		//FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);

		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TracerEnd);
		}

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

void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{

	UParticleSystem* SelectedEffect = nullptr;

	//switch to determine which surface effect to use
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;

	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	//apply surface effect
	if (SelectedEffect)
	{
		//FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation; 
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());

	}

}
