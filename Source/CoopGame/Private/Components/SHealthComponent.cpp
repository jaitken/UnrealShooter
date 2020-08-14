// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "SGameMode.h"
#include "AI/SBouncingBot.h"
#include "SCharacter.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "Engine/World.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{

	DefaultHealth = 100;
	bIsDead = false;

	TeamNum = 255;

	SetIsReplicatedByDefault(true);

}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	//only hook if we are server
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();

		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}


	}
	
	Health = DefaultHealth;

	//if the health component belongs to the player character apply healing overtime
	AActor* MyOwner = GetOwner();
	ASCharacter* MyCharacter = Cast<ASCharacter>(MyOwner);
	if (MyCharacter)
	{
		if (MyCharacter->IsPlayerControlled()) 
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_HealSelf, this, &USHealthComponent::HealSelf, .05f, true);
		}
	}
	
}

void USHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;

	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void USHealthComponent::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage < 0.0f || bIsDead)
	{
		//UE_LOG(LogTemp, Log, TEXT("Target is already Dead"));
		return;
	}


	//friendly fire (Except if it is a bouncing bot)
	ASBouncingBot* temp = Cast<ASBouncingBot>(DamageCauser);
	if (!temp) 
	{
		if ((DamageCauser != DamagedActor) && IsFriendly(DamagedActor, DamageCauser))
		{
			//UE_LOG(LogTemp, Log, TEXT("Damage Ignored"));
			return;
		}
	}
	
	//UE_LOG(LogTemp, Log, TEXT("Health: %s"), *FString::SanitizeFloat(Health));
	//apply damage while clamping health to 0/default health
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	//Health = Health - Damage;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Damage);
	DamagedRecently = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Damage, this, &USHealthComponent::ResetDamagedRecently, 3.0f, false);
	
	//UE_LOG(LogTemp, Log, TEXT("Damage: %f"), Damage);
	//UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	//if target is dead mark the IsDead bool
	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (Health <= 0.0f)
	{
		
		ASCharacter* Killer = Cast<ASCharacter>(DamageCauser);
		if (Killer)
		{
			Killer->Money = Killer->Money + Value;
		}
		//will only happen on server, gamemode only exists on server
		ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
		
	}
	
}

void USHealthComponent::HealSelf()
{
	
	if (!DamagedRecently && !bIsDead && !(Health > DefaultHealth))
	{
		//UE_LOG(LogTemp, Log, TEXT("SelfHeal Active"));
		Health += 1;
	}
}

void USHealthComponent::ResetDamagedRecently()
{
	DamagedRecently = false;
}

float USHealthComponent::GetHealth() const
{
	return Health;
}

void USHealthComponent::Heal(float HealAmount)
{

	if (HealAmount <= 0.0f || Health <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);

	//UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);



}

bool USHealthComponent::IsFriendly(AActor * ActorA, AActor * ActorB)
{
	UE_LOG(LogClass, Log, TEXT("ActorA: %s"), *ActorA->GetName());
	UE_LOG(LogClass, Log, TEXT("ActorB: %s"), *ActorB->GetName());

	//if either actor is null assume friendly
	if (ActorA == nullptr || ActorB == nullptr)
	{
		return true;
	}

	USHealthComponent* HealthCompA = Cast<USHealthComponent>(ActorA->GetComponentByClass(USHealthComponent::StaticClass()));
	USHealthComponent* HealthCompB = Cast<USHealthComponent>(ActorB->GetComponentByClass(USHealthComponent::StaticClass()));

	//if either health component is null assume friendly
	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		return true;
	}

	return (HealthCompA->TeamNum == HealthCompB->TeamNum);

}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, Health);
}


