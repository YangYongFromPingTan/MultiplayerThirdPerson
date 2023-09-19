// Fill out your copyright notice in the Description page of Project Settings.


#include "StartReloadAnimNotify.h"
#include "YCharacter.h"
UStartReloadAnimNotify::UStartReloadAnimNotify() {

}

void UStartReloadAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	AYCharacter* owner = MeshComp->GetOwner<AYCharacter>();
	if (owner == nullptr) {
		if (GEngine != nullptr) {
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Failed"));
		}
		return;
	}
	owner->SetCanFire(false);
}