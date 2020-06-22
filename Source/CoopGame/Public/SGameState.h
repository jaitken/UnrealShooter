// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"


UENUM(BlueprintType)
enum class EWaveState : uint8
{
	MainMenu,

	WaitingToStart,

	WaveInProgress,
	
	//no longer spawning new bots, waiting for players to kill remaining bots
	WaitingToComplete,

	WaveComplete,

	GameOver,

};
/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChanged(EWaveState NewState, EWaveState OldState);
	
	UPROPERTY(BlueprintReadOnly, Category = "GameState")
	EWaveState WaveState;
	
public:

	UFUNCTION(BlueprintCallable, Category = "GameState")
	void SetWaveState(EWaveState NewState);

	EWaveState GetWaveState();
	
};
