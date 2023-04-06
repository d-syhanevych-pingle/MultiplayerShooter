// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameMode/ShooterGameMode.h"
#include "GameState/ShooterGameState.h"
#include "ShooterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnPosses, AShooterPlayerController*, ShooterPlayerController);
DECLARE_DELEGATE_OneParam(FOnGameStateSetDelegate, AGameStateBase*);

UCLASS()
class MULTIPLAYERSHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	//virtual void ReceivedPlayer() override;		
	void PostInitializeComponents();
	virtual void ReceivedGameModeClass(TSubclassOf<AGameModeBase> GameModeClass) override;

public:
	//virtual void PostSeamlessTravel();
	virtual void ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass) override;

	void UpdatePlayerHealth(float Health, float MaxHealth);
	void UpdatePlayerScore(float Value);
	void UpdatePlayerDefeats(int32 Value);
	/** Display the text when player is eliminated */
	void DisplayDefeatedMsg();
	void UpdateWeaponAmmo(int32 AmmoAmount);
	void UpdateCarriedAmmo(int32 AmmoAmount);
	void UpdateWeaponType(const FString& WeaponType);
	void UpdateGrenade(int32 GrenadeAmount);

	/** Update the top score player */
	void UpdateTopScorePlayer();
	/** Update the top score */
	void UpdateTopScore();
	void RefreshHUD();

	UFUNCTION()
	void OnMatchStateChange(FName MatchState);

	FOnPawnPosses OnPawnPosses;
	FOnGameStateSetDelegate GameStateSetDelegate;
private:

	UPROPERTY()
	class AShooterHUD* ShooterHUD;

	/**
	 *	Sync implemented using gameState Timers
	 */
	//UFUNCTION(Server, Reliable)
	//void RequestServerTimeFromClient(float ClientRequestTime);

	//UFUNCTION(Client, Reliable)
	//void ReportServerTimeToClient(float ClientRequestTime, float ServerReportTime);

	//void CheckTimeSync(float DeltaTime);

	//FORCEINLINE float GetServerTime() const { return HasAuthority() ? GetWorld()->GetTimeSeconds() : GetWorld()->GetTimeSeconds() + SyncDiffTime; }


	UFUNCTION(Server, Reliable)
	void ServerPawnPosses();

	UFUNCTION(Server, Reliable)
	void ServerReadyToGame(AShooterGameState* ShooterGameState);

	//UPROPERTY(EditAnywhere, Category = Sync)
	//float SyncFreq = 5.f;
	//
	//float SyncDiffTime = 0.f;
	//float SyncRunningTime = 0.f;

	/*UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(float Warmup, float Match, float Cooldown, FName State);

	UFUNCTION(Server, Reliable)
	void ServerGetMatchState();*/
};
