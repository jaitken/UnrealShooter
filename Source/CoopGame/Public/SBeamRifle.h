// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SBeamRifle.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASBeamRifle : public ASWeapon
{
	GENERATED_BODY()

public:

	virtual void StartReload() override;

	virtual void FinishReload() override;
	
protected:

	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundCue* BetweenShotSound;

	FTimerHandle TimerHandle_TBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float TBetweenShots;

	void PlayBetweenShotSound();

};
