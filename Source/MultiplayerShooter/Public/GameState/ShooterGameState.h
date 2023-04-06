// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ShooterGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchTick, int32, TickValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchCooldownTick, int32, TickValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchWarmupTick, int32, TickValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchStateChanged, FName, MatchState);

UCLASS()
class MULTIPLAYERSHOOTER_API AShooterGameState : public AGameState
{
	GENERATED_BODY()

public: AShooterGameState();
	/** Once a player is eliminated, we then need to update the array: TopScorePlayerStates and update the TopScore player in the HUD */
	void UpdateTopScorePlayerStates(class AShooterPlayerState* PlayerState);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void DefaultTimer() override;

	void StartTimer(bool IsStart);

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 WarmupTime;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 CooldownTime;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 MatchTime;

	//UFUNCTION()
	virtual void OnRep_MatchState();

	void PlayerJoined();

	FOnMatchTick OnMatchTick;
	FOnMatchCooldownTick OnMatchCooldownTick;
	FOnMatchWarmupTick OnMatchWarmupTick;
	FOnMatchStateChanged OnMatchStateChanged;

private:
	UPROPERTY(Replicated)
	float TopScore = 0.f;

	/** The common code within OnRep_TopScore() */
	UFUNCTION()
	void OnRep_TopScore();

	/** An array contains the top score players' states */
	UPROPERTY(Replicated)
	TArray<class AShooterPlayerState*> TopScorePlayerStates;

	/** The common code within OnRep_TopScorePlayerStates() */
	UFUNCTION()
	void OnRep_TopScorePlayerStates();

	UFUNCTION()
	void OnRep_IsTimerStarted();

	UPROPERTY(ReplicatedUsing=OnRep_IsTimerStarted)
	bool IsTimerStarted = false;

	FTimerHandle TimerHandle_ShooterTickTimer;

	int32 CountJoinedPlayers = 0;

public:
	FORCEINLINE float GetTopScore() const { return TopScore; }
	FORCEINLINE const TArray<class AShooterPlayerState*>& GetTopScorePlayerStates() const { return TopScorePlayerStates; }

};
