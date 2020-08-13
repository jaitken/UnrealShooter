// Fill out your copyright notice in the Description page of Project Settings.


#include "SNormalBullet.h"
#include "Components/SphereComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/SkeletalMeshComponent.h"
#include "CoopGame/CoopGame.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

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
	SetLifeSpan(5.0f);
	
}

// Called every frame
void ASNormalBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASNormalBullet::NotifyActorBeginOverlap(AActor * OtherActor)
{
	//if actor has already been damaged by this bullet ignore
	if (OtherActor == LastDamagedActor) {
		return;
	}
	LastDamagedActor = OtherActor;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	FVector Forward = GetActorForwardVector();
	Forward = Forward * 500;

	FHitResult Hit;
	EPhysicalSurface SurfaceType = SurfaceType_Default;
	if (GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation()-Forward, GetActorLocation(), COLLISION_WEAPON, QueryParams))
	{
		DrawDebugLine(GetWorld(), GetActorLocation()-Forward, GetActorLocation(), FColor::Red, false, 5.0f, 0, 1.0f);

		SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

		float damage = 20;
		if (SurfaceType == SURFACE_FLESHVULNERABLE)
		{
			UE_LOG(LogTemp, Log, TEXT("HeadShot"));
			damage = 40;
		}
		else
			UE_LOG(LogTemp, Log, TEXT("NormalShot"));

		AActor* HitActor = Hit.GetActor();

		UGameplayStatics::ApplyPointDamage(HitActor, damage, GetActorLocation() + Forward, Hit, this->GetInstigatorController(), this->GetOwner()->GetOwner(), DamageType);

	}
	
}
