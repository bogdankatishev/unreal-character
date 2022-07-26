// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "unreal_characterCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS(config=Game)
class Aunreal_characterCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	Aunreal_characterCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

protected:
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser);

protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	bool bMovingForward;

	/** Called for side to side input */
	void MoveRight(float Value);

	bool bMovingRight;

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	/** Set movement status and running speed */
	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	bool bShiftKeyDown;

	/** Pressed down to enable sprinting */
	void ShiftKeyDown();

	/** Released down to stop sprinting */
	void ShiftKeyUp();

	/** Player Stats */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float FullHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
	float HealthPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
	float PreviousHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
	float StaminaPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
	float PreviousStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
	float redFlash;

	FTimeline MyTimeline;
	float TimelineValue;
	FTimerHandle MemberTimerHandle;

	bool bCanBeDamaged;

	/** Get Health */
	UFUNCTION(BlueprintPure, Category = "PlayerStats")
	float GetHealth();

	/** Get Health Text */
	UFUNCTION(BlueprintPure, Category = "PlayerStats")
	FText GetHealthIntText();

	/** Get Stamina */
	UFUNCTION(BlueprintPure, Category = "PlayerStats")
	float GetStamina();

	/** Get Stamina Text */
	UFUNCTION(BlueprintPure, Category = "PlayerStats")
	FText GetStaminaIntText();

	/** Damage Timer */
	UFUNCTION()
	void DamageTimer();

	/** Set Damage State */
	UFUNCTION()
	void SetDamageState();

	/** Update Health */
	UFUNCTION(BlueprintCallable, Category = "Power")
	void UpdateHealth(float HealthChange);

	/** Update Stamina */
	UFUNCTION(BlueprintCallable, Category = "Power")
	void UpdateStaminaPercentage();

	/** Play Flash */
	UFUNCTION(BlueprintPure, Category = "PlayerStats")
	bool PlayFlash();

};

