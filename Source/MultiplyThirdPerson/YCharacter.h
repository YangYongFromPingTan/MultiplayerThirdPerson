// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "YCharacter.generated.h"

UCLASS()
class MULTIPLYTHIRDPERSON_API AYCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* FireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* ReloadMontage;

	float MaxHealth;
	float CurrentHealth;
	int RemainingAmmo;
	int TotalAmmo;
	bool canFire;
	const int MagazineSize = 30;

public:
	// Sets default values for this character's properties
	AYCharacter();

protected:
	void Move(const struct FInputActionValue&);
	void Look(const struct FInputActionValue&);
	void Fire(const struct FInputActionValue&);

	void Sprint(const struct FInputActionValue& Value);
	
	void StopSprinting(const struct FInputActionValue& Value);
	
	void Reload(const struct FInputActionValue&);

public:
	
	void DoReload();
	void SetCanFire(bool value) { this->canFire = value; }

	UFUNCTION(Client, Unreliable)
	void UpdateAiming(FVector2D rotation);

	UFUNCTION(Server, Reliable)
	void DoSprint();

	UFUNCTION(Server, Reliable)
	void DoStopSprint();

	UFUNCTION(Server, Reliable)
	void ShootCheck();

	UFUNCTION(NetMulticast, Unreliable)
	void GetHitted(int damage);

	UFUNCTION(NetMulticast, Unreliable)
	void PlayFireMontage();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayAnimMontageFromServer(UAnimMontage* montage);

	UFUNCTION(Server,Unreliable)
	void CallServerToPlayMontage(UAnimMontage* montage);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	int GetTotalAmmo() { return this->TotalAmmo; }
	UFUNCTION(BlueprintCallable)
	int GetRemainingAmmo() { return this->RemainingAmmo; }
	UFUNCTION(BlueprintCallable, Category = Animation)
	float GetHealthBar() { return this->CurrentHealth/(float)MaxHealth; }
	

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
