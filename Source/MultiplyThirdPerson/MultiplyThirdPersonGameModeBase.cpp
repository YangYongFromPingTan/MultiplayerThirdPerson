// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplyThirdPersonGameModeBase.h"
#include "YCharacter.h"
AMultiplyThirdPersonGameModeBase::AMultiplyThirdPersonGameModeBase() {
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/MyContent/Protagonist/Blueprints/BP_Protagonist"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Loading %s"), TEXT("/Game/MyContent/Protagonist/Blueprints/BP_Protagonist.BP_Protagonist"));
		this->DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Protagonist lost, it should be %s ") , TEXT("/Game/MyContent/Protagonist/Blueprints/BP_Protagonist.BP_Protagonist")) ;
	}
}

void AMultiplyThirdPersonGameModeBase::BeginPlay() {
}