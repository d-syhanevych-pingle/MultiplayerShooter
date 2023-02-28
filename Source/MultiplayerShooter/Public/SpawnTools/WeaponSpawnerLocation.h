// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponSpawnerLocation.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API AWeaponSpawnerLocation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponSpawnerLocation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool GetIsExistsWeaponOnLocation() const { return IsExistsWeaponOnLocation; };
	void OnWeaponTaken() { IsExistsWeaponOnLocation = false; };
	void OnWeaponSpawned() { IsExistsWeaponOnLocation = true; };

private:
	bool IsExistsWeaponOnLocation = false;
};
