// Fill out your copyright notice in the Description page of Project Settings.


#include "SShotgunWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame/CoopGame.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "Sound/SoundCue.h"
#include "SCharacter.h"
#include "SNormalBullet.h"


void ASShotgunWeapon::Fire()
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

			//offset Eyelocation so the line trace starts more in line with muzzlelocation
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



				//PLAY FX AND SOUND
				PlayFireEffects(TracerEndPoint);
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
		//FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, MuzzleLocation);
		StartReload();
	}
}

void ASShotgunWeapon::StartReload()
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
		GetWorldTimerManager().SetTimer(TimerHandle_ReloadTime, this, &ASWeapon::FinishReload, ReloadTime, false);
	}

}

void ASShotgunWeapon::FinishReload()
{
	reloading = false;

	AActor* MyOwner = GetOwner();
	ASCharacter* MyCharacter = Cast<ASCharacter>(MyOwner);

	float NeededAmmo = MagSize - CurrentAmmo;
	if (MyCharacter->ShotgunAmmo >= NeededAmmo)
	{
		CurrentAmmo = MagSize;
		MyCharacter->ShotgunAmmo = MyCharacter->ShotgunAmmo - NeededAmmo;
	}
	else
	{
		CurrentAmmo = CurrentAmmo + MyCharacter->ShotgunAmmo;
		MyCharacter->ShotgunAmmo = 0;
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);

}


void ASShotgunWeapon::PlayPumpSound()
{
	//FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	UGameplayStatics::PlaySoundAtLocation(this, PumpSound, MuzzleLocation);

	GetWorldTimerManager().ClearTimer(TimerHandle_Pump);
}
