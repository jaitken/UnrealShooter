// Fill out your copyright notice in the Description page of Project Settings.


#include "SDBShotgun.h"
#include "DrawDebugHelpers.h"
#include "Sound/SoundCue.h"
#include "SCharacter.h"
#include "CoopGame/CoopGame.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"



void ASDBShotgun::Fire()
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

			float Yaw = EyeRotation.Yaw;

			for (int i = 0; i < PelletCount; i++)
			{
				//switch statment/offset vector to give a nice even bullet spread
				FVector offset = FVector(0, 0, 0);
				if ((Yaw >= 45 && Yaw < 135) || (Yaw >= 225 && Yaw < 315)) 
				{
					switch (i)
					{
					default:
						break;
					case 0:
						offset = FVector(-150, 0, 75);
						break;
					case 1:
						offset = FVector(0, 0, 75);
						break;
					case 2:
						offset = FVector(150, 0, 75);
						break;
					case 3:
						offset = FVector(-200, 0, 0);
						break;
					case 4:
						offset = FVector(-75, 0, 0);
						break;
					case 5:
						offset = FVector(75, 0, 0);
						break;
					case 6:
						offset = FVector(200, 0, 0);
						break;
					case 7:
						offset = FVector(-150, 0, -75);
						break;
					case 8:
						offset = FVector(0, 0, -75);
						break;
					case 9:
						offset = FVector(150, 0, -75);
						break;

					}

				}
				else
				{
					switch (i)
					{
					default:
						break;
					case 0:
						offset = FVector(0, -150, 75);
						break;
					case 1:
						offset = FVector(0, 0, 75);
						break;
					case 2:
						offset = FVector(0, 150, 75);
						break;
					case 3:
						offset = FVector(0, -200, 0);
						break;
					case 4:
						offset = FVector(0, -75, 0);
						break;
					case 5:
						offset = FVector(0, 75, 0);
						break;
					case 6:
						offset = FVector(0, 200, 0);
						break;
					case 7:
						offset = FVector(0, -150, -75);
						break;
					case 8:
						offset = FVector(0, 0, -75);
						break;
					case 9:
						offset = FVector(0, 150, -75);
						break;

					}

				}


				//first line trace from camera to find impact point
				FVector TraceEnd = EyeLocation + (ShotDirection * 4000);
				TraceEnd = TraceEnd + offset;
				FVector TracerEndPoint;
				EPhysicalSurface SurfaceType = SurfaceType_Default;
				FHitResult Hit;
				if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
				{
					//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Blue, false, 10.0f, 0, 1.0f);
					AActor* HitActor = Hit.GetActor();
					TracerEndPoint = Hit.ImpactPoint;
					//DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 2.0f, 12, FColor::Blue, false, 1.0f, 0, 1.0f);

					float ActualDamage = BaseDamage;
					if (SurfaceType == SURFACE_FLESHVULNERABLE)
					{
						ActualDamage *= 4.0f;
					}

					UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);
					PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

				}

				//Play FX
				PlayFireEffects(TracerEndPoint);

				//Play Sound
				UGameplayStatics::PlaySoundAtLocation(this, ShotSound, MuzzleLocation);


				//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 10.0f, 0, 2.0f);

				LastFireTime = GetWorld()->TimeSeconds;

			}
			reloading = false;
			CurrentAmmo--;


		}

	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, MuzzleLocation);
		StartReload();
	}
}

void ASDBShotgun::StartReload()
{

		//don't reload if magazine already full
		if (CurrentAmmo == MagSize)
		{
			return;
		}
		//only reload if player has ammo
		AActor* MyOwner = GetOwner();
		ASCharacter* MyCharacter = Cast<ASCharacter>(MyOwner);

		if (MyCharacter->ShotgunAmmo > 0)
		{
			//play sound
			//FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
			UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, MuzzleLocation);

			reloading = true;
			StopFire();
			GetWorldTimerManager().SetTimer(TimerHandle_ReloadTime, this, &ASDBShotgun::FinishReload, ReloadTime, false);
		}
}

void ASDBShotgun::FinishReload()
{	
		reloading = false;
		AActor* MyOwner = GetOwner();
		ASCharacter* MyCharacter = Cast<ASCharacter>(MyOwner);

		float NeededAmmo = MagSize - CurrentAmmo;
		if (MyCharacter->ShotgunAmmo >= NeededAmmo)
		{
			CurrentAmmo = CurrentAmmo + 1;
			MyCharacter->ShotgunAmmo = MyCharacter->ShotgunAmmo - 1;
		}
		else
		{
			CurrentAmmo = CurrentAmmo + 1;
			MyCharacter->ShotgunAmmo = 0;
		}

		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);

		
		if (CurrentAmmo < MagSize)
		{
			StartReload();

		}
		
		
		
	
}

