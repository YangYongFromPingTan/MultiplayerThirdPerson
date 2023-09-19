// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiplyThirdPersonGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLYTHIRDPERSON_API AMultiplyThirdPersonGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMultiplyThirdPersonGameModeBase();
protected:
	void BeginPlay() override;
};
