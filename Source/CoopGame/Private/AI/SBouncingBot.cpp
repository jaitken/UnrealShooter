// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBouncingBot.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "AI/NavigationSystemBase.h"
#include "GameFramework/Character.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "Components/SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SCharacter.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "EngineUtils.h"
#include "CoopGame/CoopGame.h"
#include "SWall.h"

// Sets default values
ASBouncingBot::ASBouncingBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.05f;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASBouncingBot::HandleTakeDamage);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);

	bUseVelocityChange = true;
	MovementForce = 1000;

	RequiredDistanceToTarget = 100;

	ExplosionRadius = 400;
	ExplosionDamage = 40;

	SelfDamageInterval = 0.25f;

	VerticalForce = 1000;
	

}

// Called when the game starts or when spawned
void ASBouncingBot::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
	{
		NextPathPoint = GetNextPathPoint();

		//make bots more responsive to player movement
		GetWorldTimerManager().SetTimer(TimerHandle_FindNextPathPoint, this, &ASBouncingBot::UpdatePath, .5f, true);

		GetWorldTimerManager().SetTimer(TimerHandle_IsTouchingGround, this, &ASBouncingBot::IsTouchingGround, .05f, true);
	}

	bSoundReady = true;

	
}

void ASBouncingBot::HandleTakeDamage(USHealthComponent * OwningHealthComp, float Health, float HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{

	//Explode when health <=0
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));

	}
	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);

	}


	if (Health <= 0.0f)
	{
		SelfDestruct();
	}

}

FVector ASBouncingBot::GetNextPathPoint()
{
	AActor* BestTarget = nullptr;
	float NearestTargetDistance = FLT_MAX;

	//Cycle through all actors in world
	for (TActorIterator<AActor> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		AActor* Actor = *ActorIterator;

		//ignore any freindly or nullptr actors
		if (Actor == nullptr || USHealthComponent::IsFriendly(this, Actor))
		{
			continue;
		}

		//assign closest actor as best target
		USHealthComponent* TestPawnHealthComp = Cast<USHealthComponent>(Actor->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp && TestPawnHealthComp->GetHealth() > 0.0f)
		{
			float Distance = (Actor->GetActorLocation() - GetActorLocation()).Size();

			if (Distance < NearestTargetDistance)
			{
				NearestTargetDistance = Distance;
				BestTarget = Actor;
			}
		}
	}

	if (BestTarget)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), BestTarget);

		if (NavPath)
		{
			if (NavPath->PathPoints.Num() > 1)
			{
				//return next point in path
				return NavPath->PathPoints[1];
			}
		}
	}

	//failed to find path
	return GetActorLocation();
}

void ASBouncingBot::UpdatePath()
{
	NextPathPoint = GetNextPathPoint();
}

void ASBouncingBot::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());

	//hide Mesh Comp 
	MeshComp->SetVisibility(false, true);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (GetLocalRole() == ROLE_Authority)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);

		//increase damage if more bots are near
		float ActualDamage = ExplosionDamage;

		UGameplayStatics::ApplyRadialDamage(this, ActualDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		//DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 4.0f, 1.0f, 2.0f);

		//destory object from scene
		SetLifeSpan(1.0f);
	}
}

void ASBouncingBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void ASBouncingBot::IsTouchingGround()
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	FHitResult Hit;

	FVector TraceEnd = GetActorLocation() + FVector(0, 0, -70);


	//DrawDebugLine(GetWorld(), GetActorLocation(), TraceEnd, FColor::Red, false, 3.0f, 0, 1.0f);

	if (GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), TraceEnd, COLLISION_GROUND, QueryParams))
	{

		//Blocking hit, process damage
		AActor* HitActor = Hit.GetActor();
		
		ASWall* temp = Cast<ASWall>(HitActor);
		if (temp) 
		{
			return;
		}
		//UE_LOG(LogTemp, Warning, TEXT("object grounded"))
		bGrounded = true;
		return;


	}
	else 
	{
		//UE_LOG(LogTemp, Warning, TEXT("object in air"))
		bGrounded = false;
	}
		


	
}

void ASBouncingBot::PlaySound()
{
	if (bSoundReady) {
		UE_LOG(LogTemp, Warning, TEXT("BOUNCE SOUND PLAYED"));
		UGameplayStatics::PlaySoundAtLocation(this, BounceSound, GetActorLocation());
		bSoundReady = false;
		GetWorldTimerManager().SetTimer(TimerHandle_BounceSound, this, &ASBouncingBot::PrepareNextSound, 0.5f, false);
	}
}

void ASBouncingBot::PrepareNextSound()
{
	bSoundReady = true;
}

// Called every frame
void ASBouncingBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority && !bExploded)
	{

		if (bGrounded) 
		{
			FVector UpForce = FVector(0, 0, VerticalForce);

			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();
			ForceDirection *= MovementForce;

			ForceDirection = ForceDirection + UpForce;

			//keep moving towards target
			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
			//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Red, false, 0.1f, 0, 1.0f);
			
			//Play Sound 
			PlaySound();


			DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Red, false, 0.1f, 1.0f);
		}
	
	}

}

void ASBouncingBot::NotifyActorBeginOverlap(AActor * OtherActor)
{

	Super::NotifyActorBeginOverlap(OtherActor);

	if (!bStartedSelfDestruction && !bExploded)
	{
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);

		if (PlayerPawn && PlayerPawn->bIsPlayer)
		{
			//overlapped player
			if (GetLocalRole() == ROLE_Authority)
			{
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASBouncingBot::DamageSelf, SelfDamageInterval, true, 0.0f);

			}

			bStartedSelfDestruction = true;

			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
		}
	}

}


