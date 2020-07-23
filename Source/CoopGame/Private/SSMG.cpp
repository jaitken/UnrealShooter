// Fill out your copyright notice in the Description page of Project Settings.


#include "SSMG.h"
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

void ASSMG::Fire()
{

	if (CurrentAmmo > 0)
	{

		//trace a line from pawn eyes to crosshair location(center screen)
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{

			FVector EyeLocation;
			FRotator EyeRotation;

			MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

			FVector ShotDirection = EyeRotation.Vector();

			//bullet spread
			FDateTime DT = FDateTime::Now();
			int32 CurrTimeMS = ((DT.GetHour() * 60 * 60) + (DT.GetMinute() * 60) + DT.GetSecond()) * 1000 + DT.GetMillisecond();
			int32 ContinousFireTime = CurrTimeMS - FireStartTime;
			UE_LOG(LogTemp, Warning, TEXT("Conitous Fire Time:  %d"), ContinousFireTime);
			float BulletSpread = (ContinousFireTime / BulletSpreadTimer) * MaxBulletSpread;
			BulletSpread = BulletSpread + MinBulletSpread;
			if (BulletSpread > MaxBulletSpread)
			{
				BulletSpread = MaxBulletSpread;
			}
			float HalfRad = FMath::DegreesToRadians(BulletSpread);
			ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);


			FVector TraceEnd = EyeLocation + (ShotDirection * 10000);


			//offset Eyelocation so the line trace starts more in line with muzzlelocation
			EyeLocation = EyeLocation + (ShotDirection * 175);

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(MyOwner);
			QueryParams.AddIgnoredActor(this);
			QueryParams.bTraceComplex = true;
			QueryParams.bReturnPhysicalMaterial = true;


			FVector TracerEndPoint = TraceEnd;

			EPhysicalSurface SurfaceType = SurfaceType_Default;


			FHitResult Hit;

			if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
			{

				//Blocking hit, process damage
				AActor* HitActor = Hit.GetActor();

				//determine surface type
				SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

				float ActualDamage = BaseDamage;
				if (SurfaceType == SURFACE_FLESHVULNERABLE)
				{
					ActualDamage *= 4.0f;
				}

				UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

				PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

				TracerEndPoint = Hit.ImpactPoint;
			}


			PlayFireEffects(TracerEndPoint);

			//Play Sound 
			FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
			UGameplayStatics::PlaySoundAtLocation(this, ShotSound, MuzzleLocation);


		
			//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 3.0f, 0, 1.0f);

			LastFireTime = GetWorld()->TimeSeconds;
			CurrentAmmo--;



		}

	}
	else
	{
		//play dry fire sound and start reload
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, MuzzleLocation);
		StartReload();
	}

}

void ASSMG::StartReload()
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
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, MuzzleLocation);

		reloading = true;
		StopFire();
		GetWorldTimerManager().SetTimer(TimerHandle_ReloadTime, this, &ASSMG::FinishReload, ReloadTime, false);
	}

}

void ASSMG::FinishReload()
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
