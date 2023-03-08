// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/ShooterGameMode.h"
#include "SpawnTools/ItemsSpawner.h"
#include "FindAndShootGameMode.generated.h"

/**
 * GameMode Description: Players search for weapons and pickups in the level and fight with it
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AFindAndShootGameMode : public AShooterGameMode
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Match Parameters")
	TSubclassOf<class AItemsSpawner> ItemsSpawner;

private:
	AItemsSpawner* SpawnerItems;
};
