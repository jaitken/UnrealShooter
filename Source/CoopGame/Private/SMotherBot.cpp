// Fill out your copyright notice in the Description page of Project Settings.


#include "SMotherBot.h"
#include "NavigationPath.h"
#include "AI/NavigationSystemBase.h"
#include "NavigationSystem.h"
#include "Components/SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"


// Sets default values
ASMotherBot::ASMotherBot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASMotherBot::HandleTakeDamage);

	bUseVelocityChange = true;

	TimeToSpawnBot = 5.0f;


}

// Called when the game starts or when spawned
void ASMotherBot::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_FindNextPathPoint, this, &ASMotherBot::UpdatePath, .5f, true);

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBot, this, &ASMotherBot::SpawnBot, TimeToSpawnBot, true);
	
}

FVector ASMotherBot::GetNextPathPoint()
{
	AActor* BestTarget = nullptr;
	float NearestTargetDistance = FLT_MAX;


	for (TActorIterator<AActor> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		AActor* Actor = *ActorIterator;

		if (Actor == nullptr || USHealthComponent::IsFriendly(this, Actor))
		{
			continue;
		}

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
		FVector offset = FVector(0, 0, 200);
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation()-offset, BestTarget);
		DrawDebugSphere(GetWorld(), GetActorLocation() - offset, 20, 12, FColor::Blue, false, 1.0f, 1.0f);
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

void ASMotherBot::UpdatePath()
{
	NextPathPoint = GetNextPathPoint();
}

void ASMotherBot::HandleTakeDamage(USHealthComponent * OwningHealthComp, float Health, float HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
}

void ASMotherBot::SpawnBot()
{
	BlueprintSpawnBot();
}

// Called every frame
void ASMotherBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority && !bExploded)
	{
		//NextPathPoint = GetNextPathPoint();
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= MovementForce;

		//keep moving towards target
		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Red, false, 0.0f, 0, 1.0f);


		DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Red, false, 1.0f, 1.0f);
	}

}

