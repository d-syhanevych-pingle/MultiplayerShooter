// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameMode.h"
#include "GameFramework/GameState.h"
#include "GeneralProjectSettings.h"
#include "DeveloperSettings/GameMapsModesDeveloperSettings.h"

#include "UObject/SoftObjectPath.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if (NumberOfPlayers == 2)
	{
		UWorld* World = GetWorld();
		if (World)
		{			
			bUseSeamlessTravel = true;
			FString TravelURL = GetRandomTravelPath();
			UE_LOG(LogTemp, Log, TEXT("ServerTravel - Travel to: %s"), *TravelURL);

			World->ServerTravel(TravelURL);
		}
	}
}

const FString ALobbyGameMode::GetRandomTravelPath()
{
	/* Get Random Map */
	const TArray<FMapDescription> AvaliableMaps = GetDefault<UGameMapsModesDeveloperSettings>()->AvaliableMaps;
	int32 MapIndex = FMath::RandRange(0, AvaliableMaps.Num() - 1);
	FString MapPath = AvaliableMaps[MapIndex].Map.GetLongPackageName();

	/* Get Random GameMode by Map*/
	const TArray<TSoftClassPtr<AShooterGameMode>> AvaliableGameModes = AvaliableMaps[MapIndex].AvaliableGameModes;
	int32 GameModeIndex = FMath::RandRange(0, AvaliableGameModes.Num() - 1);
	FString GameModeName = AvaliableGameModes[GameModeIndex].GetAssetName();
	GameModeName = AvaliableGameModes[GameModeIndex].GetLongPackageName();
	GameModeName = AvaliableGameModes[GameModeIndex].ToSoftObjectPath().GetAssetPathString();

	return FString::Printf(TEXT("%s?game=%s?listen"), *MapPath, *GameModeName);
}
