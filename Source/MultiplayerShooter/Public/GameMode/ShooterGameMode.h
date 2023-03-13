// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ShooterGameMode.generated.h"

/** Add a new custom match state in the GameMode */
namespace MatchState
{
	extern const FName Cooldown;
}

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AShooterGameMode();
	virtual void PlayerEliminated(class AMainCharacter* EliminatedCharacter, class AShooterPlayerController* VictimController, class AShooterPlayerController* AttackerController);
	virtual void RequestRespawn(class AMainCharacter* EliminatedCharacter, class AController* EliminatedController);
protected:
	virtual void OnMatchStateSet() override;
	void PostLogin(APlayerController* NewPlayer);

	FTimerManager* TimerManager;
	FTimerHandle TimerHandle_ChangeMatchState;
	
private:	
	/** Warmup time before starting the game */
	UPROPERTY(EditDefaultsOnly, Category = "Match Parameters")
	float WarmupTime = 10.f;

	/** Match time when MatchState is InProgress */
	UPROPERTY(EditDefaultsOnly, Category = "Match Parameters")
	float MatchTime = 10.f;

	/** Cooldown time when MatchState is InProgress and the match countdown has finished */
	UPROPERTY(EditDefaultsOnly, Category = "Match Parameters")
	float CooldownTime = 10.f;

	/** Countdown time since the players have entered the map */
	float CountdownTime = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Match Parameters")
	int32 MaxCountOfPlayes = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Match Parameters")
	bool CanJoinWhenMatchIsInProgress = false;

	UPROPERTY(EditDefaultsOnly, Category = "Match Parameters")
	int32 CountOfBots = 0; /* not use now */

	void FinishCurrentMatch();
	void RestarthCurrentGame();
	void StartCurrentMatch();

public:

	FORCEINLINE int32 GetMaxCountOfPlayes() const {	return MaxCountOfPlayes; }
	FORCEINLINE float GetWarmupTime() const { return WarmupTime; }
	FORCEINLINE float GetMatchTime() const { return MatchTime; }
	FORCEINLINE float GetCooldownTime() const { return CooldownTime; }
};
