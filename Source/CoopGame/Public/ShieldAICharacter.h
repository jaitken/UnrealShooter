// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCharacter.h"
#include "ShieldAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API AShieldAICharacter : public ASCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShieldAICharacter();

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<AActor> StarterShield;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName SheildAttachSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AActor* Shield;
	
};
