// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SDBShotgun.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASDBShotgun : public ASWeapon
{
	GENERATED_BODY()

public:

	virtual void StartReload() override;

	virtual void FinishReload() override;
	
protected:
	
	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float PelletCount;
};
