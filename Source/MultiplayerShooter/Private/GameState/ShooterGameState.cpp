// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/ShooterGameState.h"
#include "GameMode/ShooterGameMode.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/ShooterPlayerController.h"
#include "PlayerState/ShooterPlayerState.h"

void AShooterGameState::UpdateTopScorePlayerStates(AShooterPlayerState* PlayerState)
{
	if (!PlayerState) return;
	
	if (TopScorePlayerStates.Num() == 0)
	{
		TopScorePlayerStates.AddUnique(PlayerState);
		TopScore = PlayerState->GetScore();
		HandleTopScore();
		HandleTopScorePlayerStates();
	}
	else if (TopScore == PlayerState->GetScore())
	{
		TopScorePlayerStates.AddUnique(PlayerState);
		HandleTopScore();
		HandleTopScorePlayerStates();
	}
	else if (TopScore < PlayerState->GetScore())
	{
		TopScorePlayerStates.Empty();
		TopScorePlayerStates.AddUnique(PlayerState);
		TopScore = PlayerState->GetScore();
		HandleTopScore();
		HandleTopScorePlayerStates();
	}
}

void AShooterGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AShooterGameState, WarmupTime, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AShooterGameState, CooldownTime, COND_InitialOnly);

	//StartTimer();
}

void AShooterGameState::StartTimer()
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	TimerManager.SetTimer(TimerHandle_DefaultTimer, this, &AShooterGameState::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation() / GetWorldSettings()->DemoPlayTimeDilation, true);
}

/*Timers logic moved to here*/
void AShooterGameState::OnRep_ElapsedTime()
{
	OnMatchTick.Broadcast(ElapsedTime);
}

void AShooterGameState::OnRep_WarmupTime()
{
	OnMatchWarmupTick.Broadcast(WarmupTime);
}

void AShooterGameState::OnRep_CooldownTime()
{
	OnMatchCooldownTick.Broadcast(CooldownTime);
}

void AShooterGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();

	if (GetNetMode() != NM_DedicatedServer)
		OnMatchStateChanged.Broadcast(MatchState);
}

void AShooterGameState::DefaultTimer()
{
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		++WarmupTime;
		if (GetNetMode() != NM_DedicatedServer)
		{
			OnRep_WarmupTime();
		}
	}
	if (GetMatchState() == MatchState::Cooldown)
	{
		++CooldownTime;
		if (GetNetMode() != NM_DedicatedServer)
		{
			OnRep_CooldownTime();
		}
	}

	Super::DefaultTimer();
}

void AShooterGameState::HandleTopScore()
{
	AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!ShooterPlayerController) return;
	
	// Updating the TopScore in the HUD
	ShooterPlayerController->UpdateTopScore();
}

void AShooterGameState::HandleTopScorePlayerStates()
{
	AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!ShooterPlayerController) return;
	
	// Updating the TopScorePlayer in the HUD
	ShooterPlayerController->UpdateTopScorePlayer();
}
