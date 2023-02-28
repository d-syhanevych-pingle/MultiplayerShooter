// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/Weapon.h"
#include "EquipmentSpawner.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API AEquipmentSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEquipmentSpawner();

	UFUNCTION()
	void EquipRandomWeaponForEachCharacter();

	UFUNCTION()
	void EquipRandomWeaponForCharacter(AMainCharacter* MainCharacter);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Spawns", meta = (AllowAbstract = "true"))
	TArray<TSubclassOf<class AWeapon>> WeaponClasses;
};
