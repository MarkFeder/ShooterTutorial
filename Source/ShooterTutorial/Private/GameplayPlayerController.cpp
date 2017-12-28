// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayPlayerController.h"

AGameplayPlayerController::AGameplayPlayerController() {}

void AGameplayPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Set default values
	SetCurrentControllingDevice(EControllingDeviceEnum::CDE_Mouse);
		
	MouseSensitivityMin = 0.1f;
	MouseSensitivityMax = 2.0f;
	MouseSensitivityCurrent = 1.0f;

	TouchSensitivityMin = 15.0f;
	TouchSensitivityMax = 5.0f;
	TouchSensitivityCurrent = 10.0f;

	GyroSensitivityMin = 20.0f;
	GyroSensitivityMax = 60.0f;
	GyroSensitivityCurrent = 40.0f;
}

void AGameplayPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	InputComponent->BindAxis("MouseX", this, &AGameplayPlayerController::MouseX);
	InputComponent->BindAxis("MouseY", this, &AGameplayPlayerController::MouseY);

	InputComponent->BindKey(EKeys::O, IE_Pressed, this, &AGameplayPlayerController::OnClickedOButton);
	InputComponent->BindKey(EKeys::I, IE_Pressed, this, &AGameplayPlayerController::OnShownWeaponSelectionMenu);

	InputComponent->BindKey(EKeys::NumPadOne, IE_Pressed, this, &AGameplayPlayerController::OnPressedOneButton);
	InputComponent->BindKey(EKeys::NumPadTwo, IE_Pressed, this, &AGameplayPlayerController::OnPressedTwoButton);
	InputComponent->BindKey(EKeys::NumPadThree, IE_Pressed, this, &AGameplayPlayerController::OnPressedThreeButton);
	InputComponent->BindKey(EKeys::R, IE_Pressed, this, &AGameplayPlayerController::OnPressedRButton);
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AGameplayPlayerController::OnPressedLeftMouseButton);
}

bool AGameplayPlayerController::InputMotion(const FVector & Tilt, const FVector & RotationRate, const FVector & Gravity, const FVector & Acceleration)
{
	if (this->CurrentControllingDevice != EControllingDeviceEnum::CDE_Gyro)
	{
		UE_LOG(LogTemp, Warning, TEXT("InputMotion:: this->CurrentControllingDevice is not of type CDE_GYRO"))
		return false;
	}

	FVector SensitiveTilt = GyroSensitivityCurrent * Tilt;
	FVector Result = LastTilt - SensitiveTilt;
	AddPitchInput(Result.Z);
	AddYawInput(Result.X * (-1.0f));
	LastTilt = SensitiveTilt;
	return true;
}

bool AGameplayPlayerController::InputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D & TouchLocation, FDateTime DeviceTimestamp, uint32 TouchpadIndex)
{
	bool bResult = false;

	switch (Type)
	{
		case ETouchType::Began:
		{
			if (TouchpadIndex == 0)
			{
				LastTouch = TouchLocation;
				bResult = true;
			}

			break;
		}
		case ETouchType::Moved:
		{
			if (TouchpadIndex == 0)
			{
				float TouchXLocation = TouchLocation.X;
				float TouchYLocation = TouchLocation.Y;

				float DiffLastTouchX = TouchXLocation - LastTouch.X;
				float DiffLastTouchY = TouchYLocation - LastTouch.Y;

				AddYawInput(DiffLastTouchX / TouchSensitivityCurrent);
				AddPitchInput(DiffLastTouchY / TouchSensitivityCurrent);

				LastTouch = TouchLocation;
				bResult = true;
			}
			
			break;
		}

	}

	return bResult;
}

void AGameplayPlayerController::MouseX(float Value)
{
	if (this->CurrentControllingDevice == EControllingDeviceEnum::CDE_Mouse && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(AlwaysAddKey, 5.f, FColor::Yellow, FString("MouseX: ") + FString::SanitizeFloat(Value));
		AddYawInput(Value * MouseSensitivityCurrent);
	}
}

void AGameplayPlayerController::MouseY(float Value)
{
	if (this->CurrentControllingDevice == EControllingDeviceEnum::CDE_Mouse && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(AlwaysAddKey, 5.f, FColor::Blue, FString("MouseY: ") + FString::SanitizeFloat(Value));
		AddPitchInput(Value * MouseSensitivityCurrent);
	}
}

void AGameplayPlayerController::OnPressedOneButton()
{
	AGameplayPlayerCharacter* GameplayPlayerCharacter = this->GetGameplayPlayerCharacter();
	check(GameplayPlayerCharacter);

	GameplayPlayerCharacter->EquipWeapon(GameplayPlayerCharacter->WeaponSlot1);
}

void AGameplayPlayerController::OnPressedTwoButton()
{
	AGameplayPlayerCharacter* GameplayPlayerCharacter = this->GetGameplayPlayerCharacter();
	check(GameplayPlayerCharacter);

	GameplayPlayerCharacter->EquipWeapon(GameplayPlayerCharacter->WeaponSlot2);
}

void AGameplayPlayerController::OnPressedThreeButton()
{
	AGameplayPlayerCharacter* GameplayPlayerCharacter = this->GetGameplayPlayerCharacter();
	check(GameplayPlayerCharacter);

	GameplayPlayerCharacter->EquipWeapon(GameplayPlayerCharacter->WeaponSlot3);
}

void AGameplayPlayerController::OnClickedOButton()
{
	if (!this->WChangeSensitivityMenu)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnClickedOButton:: ChangeSensitivityMenu was not set"))
		return;
	}

	// Get BP Widget object and add it to the viewport
	this->ChangeSensitivityMenu = CreateWidget<UUserWidget>(this, this->WChangeSensitivityMenu);
	if (this->ChangeSensitivityMenu)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);
		SetInputMode(InputMode);

		bShowMouseCursor = true;
		this->ChangeSensitivityMenu->AddToViewport();
	}
}

void AGameplayPlayerController::OnPressedRButton()
{
	AGameplayPlayerCharacter* GameplayPlayerCharacter = this->GetGameplayPlayerCharacter();
	check(GameplayPlayerCharacter);

	if (!(GameplayPlayerCharacter->bIsReloading || !GameplayPlayerCharacter->bIsChangingWeapon))
	{
		bool bHaveAmmo = false;
		bool bMagIsFull = false;
		GameplayPlayerCharacter->CurrentWeapon->HaveAmmoInMag(bHaveAmmo, bMagIsFull);

		if (bMagIsFull)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnPressedLeftMouseButton:: Mag is full; GameplayPlayerCharacter can't reload weapon"))
				return;
		}

		GameplayPlayerCharacter->ReloadWeapon();
	}
}

void AGameplayPlayerController::OnPressedLeftMouseButton()
{
	AGameplayPlayerCharacter* GameplayPlayerCharacter = this->GetGameplayPlayerCharacter();
	check(GameplayPlayerCharacter);

	GameplayPlayerCharacter->FireWeapon();
}

void AGameplayPlayerController::OnShownWeaponSelectionMenu()
{
	if (!this->WWeaponSelection)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnShownWeaponSelectionMenu:: WeaponSelection was not set"))
		return;
	}

	// Get BP Widget object and add it to the viewport
	this->WeaponSelectionMenu = CreateWidget<UUserWidget>(this, this->WWeaponSelection);
	if (this->WeaponSelectionMenu)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);
		SetInputMode(InputMode);

		bShowMouseCursor = true;
		this->WeaponSelectionMenu->AddToViewport();
	}
}

void AGameplayPlayerController::SetCurrentControllingDevice(const EControllingDeviceEnum NewCurrent)
{
	if (this->CurrentControllingDevice == NewCurrent)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetCurrentControllingDevice:: NewCurrent is the same as this->CurrentControllingDevice"))
		return;
	}

	this->CurrentControllingDevice = NewCurrent;
}

float AGameplayPlayerController::GetSensitivity(const EControllingDeviceEnum WhichDevice, const bool bCurrentDevice)
{
	float LocalSensitivity = 0.0f;

	if (bCurrentDevice)
	{
		if (this->CurrentControllingDevice == EControllingDeviceEnum::CDE_Mouse)
		{
			LocalSensitivity = MouseSensitivityCurrent;
		}
		else if (this->CurrentControllingDevice == EControllingDeviceEnum::CDE_Touch)
		{
			LocalSensitivity = TouchSensitivityCurrent;
		}
		else
		{
			LocalSensitivity = GyroSensitivityCurrent;
		}
	}
	else
	{
		if (WhichDevice == EControllingDeviceEnum::CDE_Mouse)
		{
			LocalSensitivity = MouseSensitivityCurrent;
		}
		else if (WhichDevice == EControllingDeviceEnum::CDE_Touch)
		{
			LocalSensitivity = TouchSensitivityCurrent;
		}
		else
		{
			LocalSensitivity = GyroSensitivityCurrent;
		}
	}

	return LocalSensitivity;
}

void AGameplayPlayerController::SetSensitivity(const EControllingDeviceEnum WhichDevice, const float newSensitivity)
{
	if (WhichDevice == EControllingDeviceEnum::CDE_Mouse)
	{
		MouseSensitivityCurrent = newSensitivity;
	}
	else if (WhichDevice == EControllingDeviceEnum::CDE_Touch)
	{
		TouchSensitivityCurrent = newSensitivity;
	}
	else
	{
		GyroSensitivityCurrent = newSensitivity;
	}
}
