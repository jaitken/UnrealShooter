// Fill out your copyright notice in the Description page of Project Settings.


#include "SShotgunWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame/CoopGame.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "Sound/SoundCue.h"


void ASShotgunWeapon::Fire()
{

	if (CurrentAmmo > 0)
	{

		//trace a line from pawn eyes to crosshair location(center screen)
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

			//offset Eyelocation so the line trace starts more in line with muzzlelocation
			EyeLocation = EyeLocation + (ShotDirection * 175);


			//bullet spread
			for (int i = 0; i < PelletCount; i++)
			{

				float HalfRad = FMath::DegreesToRadians(BulletSpread);
				ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

				FVector TraceEnd = EyeLocation + (ShotDirection * 3000);

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

				//Play FX
				PlayFireEffects(TracerEndPoint);

				//Play Sound
				FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
				UGameplayStatics::PlaySoundAtLocation(this, ShotSound, MuzzleLocation);
				GetWorldTimerManager().SetTimer(TimerHandle_Pump, this, &ASShotgunWeapon::PlayPumpSound, TimeBeforePump, false);

			
				//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 10.0f, 0, 1.0f);

				LastFireTime = GetWorld()->TimeSeconds;

			}
			CurrentAmmo--;
			

		}

	}
	else
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, MuzzleLocation);
		StartReload();
	}
}

void ASShotgunWeapon::PlayPumpSound()
{
	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	UGameplayStatics::PlaySoundAtLocation(this, PumpSound, MuzzleLocation);

	GetWorldTimerManager().ClearTimer(TimerHandle_Pump);
}
