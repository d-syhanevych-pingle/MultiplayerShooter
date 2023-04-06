// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnTools/EquipmentSpawner.h"
#include "Character/MainCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEquipmentSpawner::AEquipmentSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEquipmentSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEquipmentSpawner::EquipRandomWeaponForEachCharacter()
{
	TArray<AActor*> CharacterList;
	UGameplayStatics::GetAllActorsOfClass(this, AMainCharacter::StaticClass(), CharacterList);

	for (AActor* Character : CharacterList)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(Character);
		EquipRandomWeaponForCharacter(MainCharacter);
	}
}

void AEquipmentSpawner::EquipRandomWeaponForCharacter(AMainCharacter* MainCharacter)
{
	UWorld* const World = GetWorld();
	if (!World)
		return;

	if (WeaponClasses.Num() && !MainCharacter->IsWeaponEquipped())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		int32 RandomClassIndex = FMath::RandRange(0, WeaponClasses.Num() - 1);
		UClass* WeaponClass = WeaponClasses[RandomClassIndex];

		if (WeaponClass)
		{
			AWeapon* SpawnedWeapon = World->SpawnActor<AWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);
			if (SpawnedWeapon)
			{
				MainCharacter->EquipWeapon(SpawnedWeapon);
				// Attach the weapon to the character's mesh component
				//SpawnedWeapon->AttachToComponent(MainCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
			}
		}
	}
}