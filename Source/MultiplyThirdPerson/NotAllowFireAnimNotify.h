// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NotAllowFireAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLYTHIRDPERSON_API UNotAllowFireAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
