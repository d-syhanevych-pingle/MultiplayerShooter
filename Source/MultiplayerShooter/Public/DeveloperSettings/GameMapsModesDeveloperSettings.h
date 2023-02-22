// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameMode/ShooterGameMode.h"
#include "GameMapsModesDeveloperSettings.generated.h"

/*
 * Struct for matching maps and game modes
 */
USTRUCT(BlueprintType)
struct FMapDescription
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowedClasses = "/Script/Engine.World"))
	FSoftObjectPath Map;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintBaseOnly))
	TArray<TSoftClassPtr<AShooterGameMode>> AvaliableGameModes;
};

/**
 * Settings for set available maps and gamemodes in project settings
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Maps and Modes Settings"))
class MULTIPLAYERSHOOTER_API UGameMapsModesDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TArray<FMapDescription> AvaliableMaps;

};
