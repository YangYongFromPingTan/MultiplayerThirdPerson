// Fill out your copyright notice in the Description page of Project Settings.


#include "YCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AYCharacter::AYCharacter():MaxHealth(100.f), CurrentHealth(100.f), RemainingAmmo(30), TotalAmmo(90), canFire(true)
{

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	GetCharacterMovement()->JumpZVelocity = 450.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 200.f;//100 for walking, 750 for sprinting;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	this->SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	this->CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	this->SpringArmComp->SetupAttachment(this->RootComponent);
	this->CameraComp->SetupAttachment(this->SpringArmComp);

	SpringArmComp->TargetArmLength = 400.f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp->bUsePawnControlRotation = false;

	this->GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	this->GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

}

// Called when the game starts or when spawned
void AYCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(this->Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(this->DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void AYCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AYCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		//Jumping
		EnhancedInputComponent->BindAction(this->JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(this->JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(this->MoveAction, ETriggerEvent::Triggered, this, &AYCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(this->LookAction, ETriggerEvent::Triggered, this, &AYCharacter::Look);

		EnhancedInputComponent->BindAction(this->FireAction, ETriggerEvent::Triggered, this, &AYCharacter::Fire);

		EnhancedInputComponent->BindAction(this->SprintAction, ETriggerEvent::Triggered, this, &AYCharacter::Sprint);
		EnhancedInputComponent->BindAction(this->SprintAction, ETriggerEvent::Completed, this, &AYCharacter::StopSprinting);
		EnhancedInputComponent->BindAction(this->ReloadAction, ETriggerEvent::Triggered, this, &AYCharacter::Reload);
	}
}

void AYCharacter::Move(const struct FInputActionValue& Value) 
{
	if (this->GetMovementComponent()->IsFalling()) {
		return;
	}
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (this->Controller != nullptr) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		//ForwardDirection = GetActorForwardVector();
		//RightDirection = GetActorRightVector();
		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

	}

}

void AYCharacter::Look(const struct FInputActionValue& Value)
{

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
	if (HasAuthority()) {
		UpdateAiming(LookAxisVector);
	}
}

void AYCharacter::Fire(const struct FInputActionValue& Value)
{
	if (!this->canFire) {
		return;
	}
	if (this->RemainingAmmo > 0) {

		PlayFireMontage_Implementation();
		PlayFireMontage();

		this->RemainingAmmo--;


		ShootCheck();



	}
	else {
		UE_LOG(LogTemp, Error, TEXT("No Ammo"));
	}
}

void AYCharacter::Sprint(const struct FInputActionValue& Value)
{
	this->DoSprint();
	if (!HasAuthority()) {
		this->DoSprint_Implementation();
	}
}

void AYCharacter::StopSprinting(const struct FInputActionValue& Value)
{
	this->DoStopSprint();
	if (!HasAuthority()) {
		this->DoStopSprint_Implementation();
	}
}

void AYCharacter::Reload(const struct FInputActionValue& Value)
{
	if (ReloadMontage == nullptr) {
		return;
	}
	if (this->TotalAmmo > 0 && this->RemainingAmmo < this->MagazineSize) {
		if (!HasAuthority()){
			CallServerToPlayMontage(this->ReloadMontage);
		}
		else {
			PlayAnimMontageFromServer(this->ReloadMontage);
		}
	}
}

void AYCharacter::DoReload() {
	if (TotalAmmo > (MagazineSize - RemainingAmmo)) {
		TotalAmmo -= (MagazineSize - RemainingAmmo);
		RemainingAmmo += (MagazineSize - RemainingAmmo);
	}
	else {
		RemainingAmmo += TotalAmmo;
		TotalAmmo = 0;
	}
	
}

//not working
void AYCharacter::UpdateAiming_Implementation(FVector2D Rotation)
{
	if (Controller != nullptr)
	{
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		PC->AddYawInput(Rotation.X);
		PC->AddPitchInput(Rotation.Y);
	}
}

void AYCharacter::DoSprint_Implementation() {
	this->SetCanFire(false);
	auto movement = this->GetCharacterMovement();
	movement->MaxWalkSpeed = 750;
}

void AYCharacter::DoStopSprint_Implementation() {
	this->SetCanFire(true);
	auto movement = this->GetCharacterMovement();
	movement->MaxWalkSpeed = 200;
}

void AYCharacter::ShootCheck_Implementation() {
	FHitResult hit;
	FVector direction = this->GetControlRotation().Vector();
	FVector traceStart = this->CameraComp->GetComponentLocation();
	FVector traceEnd = traceStart + direction * 1000.f;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);
	bool isHit = GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECC_Pawn, queryParams);
	if (isHit)
	{
		AYCharacter* enemy = Cast<AYCharacter>(hit.GetActor());
		if (enemy == nullptr) {
			return;
		}
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("hit"));
	}
}

void AYCharacter::GetHitted_Implementation(int damage) {

}

void AYCharacter::PlayFireMontage_Implementation()
{
	UAnimInstance* instance = this->GetMesh()->GetAnimInstance();
	float seconds = instance->Montage_Play(this->FireMontage);
}

void AYCharacter::PlayAnimMontageFromServer_Implementation(UAnimMontage* montage) {
	UAnimInstance* instance = this->GetMesh()->GetAnimInstance();
	if (instance != nullptr) {
		float seconds = instance->Montage_Play(montage);
	}
}

void AYCharacter::CallServerToPlayMontage_Implementation(UAnimMontage* montage) {
	PlayAnimMontageFromServer(montage);
}