// Fill out your copyright notice in the Description page of Project Settings.


#include "NotAllowFireAnimNotify.h"
#include "YCharacter.h"

void UNotAllowFireAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	AYCharacter* owner = MeshComp->GetOwner<AYCharacter>();
	if (owner == nullptr) {
		if (GEngine != nullptr) {
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Error!"));
		}
		return;
	}
	owner->SetCanFire(true);
}