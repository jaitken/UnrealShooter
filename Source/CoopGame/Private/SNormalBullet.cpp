// Fill out your copyright notice in the Description page of Project Settings.


#include "SNormalBullet.h"
#include "Components/SphereComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/SkeletalMeshComponent.h"
#include "CoopGame/CoopGame.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "SWeapon.h"
#include "Math/Vector.h"

// Sets default values
ASNormalBullet::ASNormalBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void ASNormalBullet::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(2.0f);
	
}

// Called every frame
void ASNormalBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ASNormalBullet::NotifyHit(class UPrimitiveComponent * MyComp, AActor * Other, class UPrimitiveComponent * OtherComp, bool bSelfMoved,
	                            FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit)
{
	FVector Forward = GetActorForwardVector();
	Forward = Forward * TraceAfterDistance;

	UE_LOG(LogTemp, Log, TEXT("Hit Called"));
	//DrawDebugSphere(GetWorld(), HitLocation, 5.0f, 12, FColor::Red, false, 3, 0, 1);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	FHitResult BulletHit;
	EPhysicalSurface SurfaceType = SurfaceType_Default;
	if (GetWorld()->LineTraceSingleByChannel(BulletHit, HitLocation, HitLocation + Forward, COLLISION_WEAPON, QueryParams))
	{
		//DrawDebugLine(GetWorld(), HitLocation, HitLocation + Forward, FColor::Red, false, 5.0f, 0, 1.0f);


		SurfaceType = UPhysicalMaterial::DetermineSurfaceType(BulletHit.PhysMaterial.Get());

		float ActualDamage = Damage;
		if (SurfaceType == SURFACE_FLESHVULNERABLE)
		{
			//UE_LOG(LogTemp, Log, TEXT("HeadShot"));
			ActualDamage *= 4;
		}
		else {
			//UE_LOG(LogTemp, Log, TEXT("NormalShot"));
		}

		AActor* HitActor = Hit.GetActor();

		UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, Forward, Hit, this->GetInstigatorController(), this->GetOwner()->GetOwner(), DamageType);

	}
	PlayImpactEffects(SurfaceType, HitLocation, Forward);
	Destroy();
}

void ASNormalBullet::SetDamage(float NewDamage)
{
	Damage = NewDamage;
}

void ASNormalBullet::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint, FVector ShotDirection)
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
		//FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());

	}

}