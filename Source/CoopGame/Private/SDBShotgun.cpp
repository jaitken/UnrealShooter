// Fill out your copyright notice in the Description page of Project Settings.


#include "SDBShotgun.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "SCharacter.h"
#include "CoopGame/CoopGame.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "SNormalBullet.h"




void ASDBShotgun::Fire()
{

	if (CurrentAmmo > 0)
	{


		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{


			FVector EyeLocation;
			FRotator EyeRotation;
			MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
			FVector ShotDirection = EyeRotation.Vector();
			

			float Yaw = EyeRotation.Yaw;
			for (int i = 0; i < PelletCount; i++)
			{


				//switch statment/offset vector to give a even bullet spread no matter the direction the player is facing
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
				TraceEnd = TraceEnd + offset;
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


				//PLAY FX AND SET LAST FIRE TIME
				PlayFireEffects(TracerEndPoint);
				UGameplayStatics::PlaySoundAtLocation(this, ShotSound, MuzzleLocation);
				LastFireTime = GetWorld()->TimeSeconds;


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

