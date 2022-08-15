// Copyright Epic Games, Inc. All Rights Reserved.

#include "unreal_characterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "AttributeComponent.h"
#include "Items/Item.h"
#include "Items/InventoryComponent.h"

//////////////////////////////////////////////////////////////////////////
// Aunreal_characterCharacter

Aunreal_characterCharacter::Aunreal_characterCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	
	bCanBeDamaged = true;

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	bShiftKeyDown = false;

	// Initialize Enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 250.f;
	MinSprintStamina = 500.f;

	bMovingForward = false;
	bMovingRight = false;

	AttributeComp = CreateDefaultSubobject<UAttributeComponent>("AttributeComp");
	Inventory = CreateDefaultSubobject<UInventoryComponent>("Inventory");
	
	Inventory->Capacity = 20;
}

void Aunreal_characterCharacter::UseItem(class  UItem* Item)
{
	if (Item)
	{
		Item->Use(this);
		Item->OnUse(this); //bp event
	}
}

void Aunreal_characterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &Aunreal_characterCharacter::OnHealthChanged);
	AttributeComp->OnStaminaChanged.AddDynamic(this, &Aunreal_characterCharacter::OnStaminaChanged);
}

void Aunreal_characterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MyTimeline.TickTimeline(DeltaTime);	

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown)
		{
			if (AttributeComp->GetStamina() - DeltaStamina <= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				AttributeComp->ApplyStaminaChange(this, -DeltaStamina);
			}
			else
			{
				AttributeComp->ApplyStaminaChange(this, -DeltaStamina);
			}
			
			if (bMovingForward || bMovingRight)
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
		}
		else // Shift key up
		{
			if (AttributeComp->GetStamina() + DeltaStamina >= AttributeComp->GetStaminaMax())
			{
				AttributeComp->ApplyStaminaChange(this, AttributeComp->GetStaminaMax());
			}
			else
			{
				AttributeComp->ApplyStaminaChange(this, DeltaStamina);
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown)
		{
			if (AttributeComp->GetStamina() - DeltaStamina <= 0.f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				AttributeComp->ApplyStaminaChange(this, -AttributeComp->GetStaminaMax());
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				AttributeComp->ApplyStaminaChange(this, -DeltaStamina);
				if (bMovingForward || bMovingRight)
				{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
			}
		}
		else // Shift key up
		{
			if (AttributeComp->GetStamina() + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				AttributeComp->ApplyStaminaChange(this, DeltaStamina);
			}
			else
			{
				AttributeComp->ApplyStaminaChange(this, DeltaStamina);
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			AttributeComp->ApplyStaminaChange(this, -AttributeComp->GetStaminaMax());
		}
		else // Shift key up
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			AttributeComp->ApplyStaminaChange(this, DeltaStamina);
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (AttributeComp->GetStamina() + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			AttributeComp->ApplyStaminaChange(this, DeltaStamina);
		}
		else
		{
			AttributeComp->ApplyStaminaChange(this, DeltaStamina);
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	default:
		;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void Aunreal_characterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &Aunreal_characterCharacter::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &Aunreal_characterCharacter::ShiftKeyUp);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &Aunreal_characterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &Aunreal_characterCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &Aunreal_characterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &Aunreal_characterCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &Aunreal_characterCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &Aunreal_characterCharacter::TouchStopped);
}

void Aunreal_characterCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void Aunreal_characterCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void Aunreal_characterCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void Aunreal_characterCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void Aunreal_characterCharacter::MoveForward(float Value)
{
	bMovingForward = false;

	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		bMovingForward = true;
	}
}

void Aunreal_characterCharacter::MoveRight(float Value)
{
	bMovingRight = false;

	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);

		bMovingRight = true;
	}
}

void Aunreal_characterCharacter::SetDamageState()
{
	bCanBeDamaged = true;
}

void Aunreal_characterCharacter::DamageTimer()
{
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &Aunreal_characterCharacter::SetDamageState, 2.0f, false);
}

bool Aunreal_characterCharacter::PlayFlash()
{
	if(redFlash)
	{
		redFlash = false;
		return true;
	}

	return false;
}

void Aunreal_characterCharacter::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void Aunreal_characterCharacter::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void Aunreal_characterCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}

}

void Aunreal_characterCharacter::OnHealthChanged(AActor* InstigatorActor, UAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	// Died
	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
	}
}

void Aunreal_characterCharacter::OnStaminaChanged(AActor* InstigatorActor, UAttributeComponent* OwningComp, float NewStamina, float Delta)
{

}