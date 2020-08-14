// Fill out your copyright notice in the Description page of Project Settings.


#include "SBeamRifle.h"
#include "SCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame/CoopGame.h"
#include "DrawDebugHelpers.h"


void ASBeamRifle::StartReload()
{

	//don't reload if magazine already full
	if (CurrentAmmo == MagSize)
	{
		return;
	}
	//only reload if player has ammo
	AActor* MyOwner = GetOwner();
	ASCharacter* MyCharacter = Cast<ASCharacter>(MyOwner);

	if (MyCharacter->SniperAmmo > 0)
	{
		//play sound
		//FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, MuzzleLocation);

		reloading = true;
		StopFire();
		GetWorldTimerManager().SetTimer(TimerHandle_ReloadTime, this, &ASWeapon::FinishReload, ReloadTime, false);
	}
}
void ASBeamRifle::FinishReload()
{

	reloading = false;

	AActor* MyOwner = GetOwner();
	ASCharacter* MyCharacter = Cast<ASCharacter>(MyOwner);

	float NeededAmmo = MagSize - CurrentAmmo;
	if (MyCharacter->SniperAmmo >= NeededAmmo)
	{
		CurrentAmmo = MagSize;
		MyCharacter->SniperAmmo = MyCharacter->SniperAmmo - NeededAmmo;
	}
	else
	{
		CurrentAmmo = CurrentAmmo + MyCharacter->SniperAmmo;
		MyCharacter->SniperAmmo = 0;
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);

}

void ASBeamRifle::Fire()
{

	if (CurrentAmmo > 0)
	{

		//trace a line from pawn eyes to crosshair location(center screen)
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			//FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
			FVector EyeLocation;
			FRotator EyeRotation;
			MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
			FVector ShotDirection = EyeRotation.Vector();

			//bullet spread
			float HalfRad = FMath::DegreesToRadians(MaxBulletSpread);
			ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(MyOwner);
			QueryParams.AddIgnoredActor(this);
			QueryParams.bTraceComplex = true;
			QueryParams.bReturnPhysicalMaterial = true;

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
			}

			//second line trace from weapon to impact point
			FHitResult Hit1;
			if (GetWorld()->LineTraceSingleByChannel(Hit1, MuzzleLocation, TracerEndPoint, COLLISION_WEAPON, QueryParams))
			{
				DrawDebugLine(GetWorld(), MuzzleLocation, TracerEndPoint, FColor::Red, false, 3.0f, 0, 1.0f);
				//Blocking hit, process damage
				AActor* HitActor = Hit1.GetActor();

				//determine surface type
				SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit1.PhysMaterial.Get());

				float ActualDamage = BaseDamage;
				if (SurfaceType == SURFACE_FLESHVULNERABLE)
				{
					ActualDamage *= 4.0f;
				}

				UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit1, MyOwner->GetInstigatorController(), MyOwner, DamageType);
				TracerEndPoint = Hit1.ImpactPoint;
			}


			PlayFireEffects(TracerEndPoint);

			//Play Sound 
			UGameplayStatics::PlaySoundAtLocation(this, ShotSound, MuzzleLocation);
			GetWorldTimerManager().SetTimer(TimerHandle_TBetweenShots, this, &ASBeamRifle::PlayBetweenShotSound, TBetweenShots, false);


			//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 3.0f, 0, 1.0f);

			LastFireTime = GetWorld()->TimeSeconds;
			CurrentAmmo--;



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

void ASBeamRifle::PlayBetweenShotSound()
{
	//FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	UGameplayStatics::PlaySoundAtLocation(this, BetweenShotSound, MuzzleLocation);

	GetWorldTimerManager().ClearTimer(TimerHandle_TBetweenShots);
}
