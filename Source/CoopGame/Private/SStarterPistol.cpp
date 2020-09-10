// Fill out your copyright notice in the Description page of Project Settings.


#include "SStarterPistol.h"
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


void ASStarterPistol::Fire()
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
			ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);
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

void ASStarterPistol::StartReload()
{
	//don't reload if magazine already full
	if (CurrentAmmo == MagSize)
	{
		return;
	}
	//only reload if player has ammo
	AActor* MyOwner = GetOwner();
	ASCharacter* MyCharacter = Cast<ASCharacter>(MyOwner);

	if (MyCharacter->LightAmmo > 0)
	{
		//play sound
		//FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, MuzzleLocation);

		reloading = true;
		StopFire();
		GetWorldTimerManager().SetTimer(TimerHandle_ReloadTime, this, &ASStarterPistol::FinishReload, ReloadTime, false);
	}

}

void ASStarterPistol::FinishReload()
{
	reloading = false;

	AActor* MyOwner = GetOwner();
	ASCharacter* MyCharacter = Cast<ASCharacter>(MyOwner);

	float NeededAmmo = MagSize - CurrentAmmo;
	if (MyCharacter->LightAmmo >= NeededAmmo)
	{
		CurrentAmmo = MagSize;
		MyCharacter->LightAmmo = MyCharacter->LightAmmo - NeededAmmo;
	}
	else
	{
		CurrentAmmo = CurrentAmmo + MyCharacter->LightAmmo;
		MyCharacter->LightAmmo = 0;
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);
}
