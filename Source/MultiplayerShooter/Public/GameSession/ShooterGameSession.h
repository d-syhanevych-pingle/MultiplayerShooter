// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "ShooterGameSession.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterGameSession : public AGameSession
{
	GENERATED_BODY()
	
public:
	void RegisterServer() override;
};
