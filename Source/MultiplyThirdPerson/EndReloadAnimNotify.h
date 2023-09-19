// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "EndReloadAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLYTHIRDPERSON_API UEndReloadAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	UEndReloadAnimNotify();
	
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
