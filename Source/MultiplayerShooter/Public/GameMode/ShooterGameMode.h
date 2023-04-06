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
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void PostSeamlessTravel();
	virtual void InitGameState();

protected:
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

	UFUNCTION()
	void FinishCurrentMatch(int32 Countdown);

	UFUNCTION()
	void RestartCurrentGame(int32 Countdown);

	UFUNCTION()
	void StartCurrentMatch(int32 Countdown);

	const FString GetRandomTravelPath();

public:

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	FORCEINLINE int32 GetMaxCountOfPlayes() const {	return MaxCountOfPlayes; }
	FORCEINLINE float GetWarmupTime() const { return WarmupTime; }
	FORCEINLINE float GetMatchTime() const { return MatchTime; }
	FORCEINLINE float GetCooldownTime() const { return CooldownTime; }
};
