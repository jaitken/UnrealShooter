// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SSMG.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASSMG : public ASWeapon
{
	GENERATED_BODY()

protected:

	virtual void Fire() override;
	
public: 

	virtual void StartReload() override;

	virtual void FinishReload() override;
};
