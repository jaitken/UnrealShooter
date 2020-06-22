// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	PowerupInterval = 0.0f;
	TotalNumOfTciks = 0;

	bIsPowerupActive = false;	

}

void ASPowerupActor::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerupTicked();

	if (TicksProcessed >= TotalNumOfTciks)
	{
		OnExpired();
		bIsPowerupActive = false;
		OnPowerupStateChanged(bIsPowerupActive);

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}


void ASPowerupActor::ActivatePowerup(AActor* ActiveFor)
{

	OnActivated(ActiveFor);

	bIsPowerupActive = true;
	OnPowerupStateChanged(bIsPowerupActive);

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, PowerupInterval, true);
	}
	else
	{
		OnTickPowerup();
	}

}
