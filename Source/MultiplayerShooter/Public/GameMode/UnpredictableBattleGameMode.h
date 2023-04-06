// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/ShooterGameMode.h"
#include "SpawnTools/EquipmentSpawner.h"
#include "UnpredictableBattleGameMode.generated.h"

/**
 *  GameMode Description: Players spawn with a random weapon in hand and an infinite amount of ammo. Weapon changes after death.
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AUnpredictableBattleGameMode : public AShooterGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	//virtual APawn* SpawnDefaultPawnAtTransform(AController* NewPlayer, const FTransform& SpawnTransform) override;
public:
	UPROPERTY(EditDefaultsOnly, Category = "Match Parameters")
	TSubclassOf<class AEquipmentSpawner> EquipmentSpawner;

private:
	AEquipmentSpawner* SpawnerEquipment;

	virtual void GenericPlayerInitialization(AController* NewPlayer) override;

	UFUNCTION()
	void EquipRandomWeaponForCharacter(AShooterPlayerController* ShooterPlayerController);
};
