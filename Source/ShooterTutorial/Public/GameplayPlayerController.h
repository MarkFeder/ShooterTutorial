// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameplayPlayerStructs.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameplayPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameplayPlayerController.generated.h"

UENUM(BlueprintType)
enum class EControllingDeviceEnum : uint8
{
	CDE_Mouse	UMETA(DisplayName="Mouse"),
	CDE_Touch	UMETA(DisplayName="Touch"),
	CDE_Gyro	UMETA(DisplayName="Gyro")
};

/**
 * This class manages all stuff related to player controller behaviour
 */
UCLASS()
class SHOOTERTUTORIAL_API AGameplayPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	/* Reference UMG asset in the editor for change sensitivity menu */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UUserWidget> WChangeSensitivityMenu;

	/* Reference UMG asset in the editor for weapon selection menu */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UUserWidget> WWeaponSelection;

	/* A reference to current device */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInput")
	EControllingDeviceEnum CurrentControllingDevice;
	
	/* Last reference to tilt vector */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerInput")
	FVector LastTilt;

	/* Last reference to touch 2d vector */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerInput")
	FVector2D LastTouch;

	/* Min sensitivity accepted for mouse device */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInput")
	float MouseSensitivityMin;

	/* Max sensitivity accepted for mouse device */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInput")
	float MouseSensitivityMax;
	
	/* Current sensitivity for mouse device */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInput")
	float MouseSensitivityCurrent;

	/* Min sensitivity accepted for touch device */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInput")
	float TouchSensitivityMin;

	/* Max sensitivity accepted for touch device */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInput")
	float TouchSensitivityMax;

	/* Current sensitivity for touch device */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInput")
	float TouchSensitivityCurrent;

	/* Current sensitivity for mouse device */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInput")
	float GyroSensitivityMin;

	/* Current sensitivity for mouse device */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInput")
	float GyroSensitivityMax;

	/* Current sensitivity for mouse device */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInput")
	float GyroSensitivityCurrent;

protected:

	/* Sets default values for this character's properties */
	AGameplayPlayerController();

	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/* Called to bind functionality to input */
	virtual void SetupInputComponent() override;

	/* Handles a motion input event */
	virtual bool InputMotion(const FVector& Tilt, const FVector& RotationRate, const FVector& Gravity, const FVector& Acceleration) override;

	/* Handles a touch input event */
	virtual bool InputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, FDateTime DeviceTimestamp, uint32 TouchpadIndex) override;

	/* Handles a mouse input event on x axis */
	virtual void MouseX(float Value);

	/* Handles a mouse input event on y axis */
	virtual void MouseY(float Value);

public:

	/* Gets current game player character instance */
	UFUNCTION(BlueprintCallable, Category = "Helpers")
	FORCEINLINE AGameplayPlayerCharacter* GetGameplayPlayerCharacter() const
	{
		return Cast<AGameplayPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}

	/* Gets current device */
	UFUNCTION(BlueprintCallable, Category = "PlayerInput")
	FORCEINLINE EControllingDeviceEnum GetCurrentControllingDevice() const
	{
		return CurrentControllingDevice;
	}

	/* Sets current device */
	UFUNCTION(BlueprintCallable, Category = "PlayerInput")
	void SetCurrentControllingDevice(const EControllingDeviceEnum NewCurrent);

	/* Gets sensitivity from device */
	UFUNCTION(BlueprintCallable, Category = "PlayerInput")
	float GetSensitivity(const EControllingDeviceEnum WhichDevice, const bool bCurrentDevice);

	/* Sets sensitivity for device */
	UFUNCTION(BlueprintCallable, Category = "PlayerInput")
	void SetSensitivity(const EControllingDeviceEnum WhichDevice, const float NewSensitivity);

private:

	const int32 AlwaysAddKey = 0;

	/* A Widget to change sensitivity menu */
	UUserWidget* ChangeSensitivityMenu;

	/* A Widget to change weapon selection menu */
	UUserWidget* WeaponSelectionMenu;

private:

	/* Handles pressed O button event */
	void OnClickedOButton();

	/* Handles first weapon button event */
	void OnPressedOneButton();

	/* Handles second weapon button event */
	void OnPressedTwoButton();

	/* Handles third weapon button event */
	void OnPressedThreeButton();

	/* Handles reload weapon button event */
	void OnPressedRButton();

	/* Handles fire weapon button event */
	void OnPressedLeftMouseButton();

	/* Handles weapon selection menu event */
	void OnShownWeaponSelectionMenu();
};
