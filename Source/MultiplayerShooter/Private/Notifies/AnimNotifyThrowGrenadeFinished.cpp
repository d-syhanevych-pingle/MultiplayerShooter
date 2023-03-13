// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/AnimNotifyThrowGrenadeFinished.h"
#include "Character/MainCharacter.h"
#include "ShooterComponents/CombatComponent.h"

void UAnimNotifyThrowGrenadeFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (AMainCharacter* Character = Cast<AMainCharacter>(MeshComp->GetOwner()))
	{
		if (UCombatComponent* CombatComponent = Cast<UCombatComponent>(Character->GetComponentByClass(UCombatComponent::StaticClass())))
		{
			CombatComponent->ThrowGrenadeAnimNotify();
		}
	}
}