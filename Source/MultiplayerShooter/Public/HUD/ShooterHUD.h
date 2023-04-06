// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterTypes/HUDPackage.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	FORCEINLINE void SetHUDPackage(FHUDPackage Package) { HUDPackage = Package; }
	FORCEINLINE void SetHUDSpread(float Spread) { HUDPackage.CrosshairsCurrentSpread = Spread; }
	
	FORCEINLINE class UCharacterOverlay* GetCharacterOverlay() const { return CharacterOverlay; }
	/** Add CharacterOverlay Widget when MatchState is InProgress */
	void AddCharacterOverlay();

	FORCEINLINE class UAnnouncementWidget* GetAnnouncement() const { return Announcement; }
	/** Add Announcement Widget when MatchState is WaitingToStart, because this MatchState is too early, so the HUD is not
	 ** available since the MatchState is set, we need to call this function in BeginPlay() */
	void AddAnnouncement();
	void UpdateAnnouncement(int32 Countdown);
	void UpdatePlayerScore(float Value);
	void UpdatePlayerDefeats(int32 Value);

	/* Refresh the HUD when the character overlay is added to the viewport or the character is respawned. */
	void Refresh();

	UFUNCTION()
	void HandleMatchState(FName MatchState);

	/** Update the top score player */
	void UpdateTopScorePlayer();
	/** Update the top score */
	void UpdateTopScore();

	void ResetHUD(AShooterGameState* ShooterGameState);

	FORCEINLINE int32 GetWarmupTime() const { return WarmupTime; }
	FORCEINLINE int32 GetMatchTime() const { return MatchTime; }
	FORCEINLINE int32 GetCooldownTime() const { return CooldownTime; }

	/** Update the warmup time before matching and update the cooldown time after the match has finished */
	UFUNCTION()
	void UpdateAnnouncementWarmup(int32 CurrentTime);
	UFUNCTION()
	void UpdateAnnouncementCooldown(int32 CurrentTime);

	/** Update the match time after matching */
	UFUNCTION()
	void UpdateMatchCountDown(int32 CurrentTime);

protected:
	virtual void BeginPlay() override;
	
private:
	/** TSubclass of the CharacterOverlay Widget */
	UPROPERTY(EditAnywhere, Category = Match)
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	/** CharacterOverlay Widget, showing the basic properties of the character */
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	/** TSubclass of the Announcement Widget */
	UPROPERTY(EditDefaultsOnly, Category = Match)
	TSubclassOf<class UUserWidget> AnnouncementClass;

	/** Announcement Widget */
	UPROPERTY()
	class UAnnouncementWidget* Announcement;
	
	/**
	 *	Draw HUD cross hairs
	 */
	void DrawCrosshairs(UTexture2D* Texture, const FVector2D& Spread);
	
	FHUDPackage HUDPackage;
	FVector2D ViewportCenter;

	/** Warmup time, MatchState on GameMode is WaitingToStart */
	UPROPERTY()
	int32 WarmupTime = 0;

	/** Match time, MatchState on GameMode is InProgress */
	UPROPERTY()
	int32 MatchTime = 0;

	/** Cooldown time when MatchState is InProgress and the match countdown has finished */
	UPROPERTY()
	int32 CooldownTime = 0;

	/** Match State, once the game mode's match state is changed, the player controller will respond */
	UPROPERTY()
	FName MatchState;
};
