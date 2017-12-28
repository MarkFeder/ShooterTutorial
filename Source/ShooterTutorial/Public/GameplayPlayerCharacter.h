// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterGameInstance.h"
#include "GameplayPlayerStructs.h"
#include "Runtime/Core/Public/Misc/App.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h" 
#include "Engine/GameInstance.h"
#include "BaseWeapon.h"
#include "GameFramework/Character.h"
#include "GameplayPlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterFireDelegate, EWeaponType, WeaponType);

UCLASS()
class AGameplayPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;


public:

	/* The backpack in which to store player's weapons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerWeapons")
	TArray<FWeaponBackpackItem> BackpackWeapons;

	/* The current player's weapon */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerWeapons")
	ABaseWeapon* CurrentWeapon;

	/* The current player's first slot available for a weapon */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerWeapons")
	ABaseWeapon* WeaponSlot1;

	/* The current player's second slot available for a weapon */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerWeapons")
	ABaseWeapon* WeaponSlot2;

	/* The current player's third slot available for a weapon */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerWeapons")
	ABaseWeapon* WeaponSlot3;

	/* Checks if this character can fire or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerWeapons")
	bool bCanFire;

	/* Tell us if hands are doing reloading animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerWeapons")
	bool bIsReloading;

	/* Tell us if we are currently changing our weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerWeapons")
	bool bIsChangingWeapon;

	/* This will be used for put weapon down and up as a changing weapon animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerWeapons")
	float WeaponPullDownPercent = 0.0f;

	/* How many items the inventory space can make room for */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerWeapons")
	float SelectedInventorySpace = 3.0f;

	/* A camera that handles FPS/TPS view */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerItems")
	UCameraComponent* Camera;

	/* A skeletal mesh component that handles FPP static mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerItems")
	USkeletalMeshComponent* FPPMesh;

	/* Curve float reference for WeaponReloadDown */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	UCurveFloat* WeaponReloadDownCurve;

	/* Curve float reference for WeaponReloadUp */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	UCurveFloat* WeaponReloadUpCurve;

	/* Curve float reference for EquipWeapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	UCurveFloat* EquipWeaponCurve;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FCharacterFireDelegate OnCharacterFireDelegate;

public:

	/* Gets current shooter game instance */
	UFUNCTION(BlueprintCallable, Category = "Helpers")
	UShooterGameInstance* GetShooterGameInstance() const;

	/* Changes current weapon */
	UFUNCTION(BlueprintCallable, Category = "PlayerWeapons")
	void SetCurrentWeapon(ABaseWeapon *Weapon);

	/* Checks if weapon can be added to selected weapon */
	UFUNCTION(BlueprintCallable, Category = "PlayerWeapons")
	bool CanAddWeaponToWeaponSelected(int32& HowManyItemsSelected);

	/* Sets the item selected on backpack */
	UFUNCTION(BlueprintCallable, Category = "PlayerWeapons")
	void SetBackpackItemSelected(const int32& BackPackItemIndex, const bool& bIsSelected, const int32& WhichSlot);

	/* Equips new weapon for the player */
	UFUNCTION(BlueprintNativeEvent, Category = "PlayerWeapons")
	void EquipWeapon(ABaseWeapon* Weapon);

	/* Reloads the equipped weapon for the player */
	UFUNCTION(BlueprintNativeEvent, Category = "PlayerWeapons")
	void ReloadWeapon();

	/* Fires the equipped weapon for the player */
	UFUNCTION(BlueprintNativeEvent, Category = "PlayerWeapons")
	void FireWeapon();

	/* Spawns the weapons and assign them to available slots */
	UFUNCTION(BlueprintCallable, Category = "PlayerWeapons")
	void SpawnWeaponsAndAssignToSlots();

	/* This is a helper function to hide and show weapons in slots */
	UFUNCTION(BlueprintCallable, Category = "PlayerWeapons")
	void ShowCurrentWeapon(const ABaseWeapon* WeaponToShow);

public:

	/* Sets default values for this character's properties */
	AGameplayPlayerCharacter();

	/* Called every frame */
	virtual void Tick(float DeltaTime) override;

	/* Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/* The function that will handle every tick of the float curve */
	UFUNCTION(Category = "Handlers")
	void OnHandleAnimPercent(float Value);

	/* The function that will handle an event during float curve execution */
	UFUNCTION(Category = "Handlers")
	void OnHandleWeaponDownEvent();

	/* The function that will handle the end of the float curve */
	UFUNCTION(Category = "Handlers")
	void OnHandleEquipWeaponFinish();

	/* The function that will handle every tick of the float curve */
	UFUNCTION(Category = "Handlers")
	void OnHandleWeaponPullDownPercent(float Value);

	/* The function that will handle the end of the float curve */
	UFUNCTION(Category = "Handlers")
	void OnFinishedHandleWeaponPullDownPercent();

	/* The function that will handle every tick of the float curve */
	UFUNCTION(Category = "Handlers")
	void OnHandleWeaponReloadDown(float Value);

	/* The function that will handle the end of the float curve */
	UFUNCTION(Category = "Handlers")
	void OnHandleWeaponReloadDownFinish();

	/* The function that will handle every tick of the float curve */
	UFUNCTION(Category = "Handlers")
	void OnHandleWeaponReloadUp(float Value);

	/* The function that will handle the end of the float curve */
	UFUNCTION(Category = "Handlers")
	void OnHandleWeaponReloadUpFinish();

	/* The function that will handle reload time of the current weapon */
	UFUNCTION(Category = "Handlers")
	void OnHandleReloadTime();

private:

	/* The timeline for equipping a weapon */
	FTimeline EquipWeaponTimeline;

	/* The timeline for reloading up a weapon */
	FTimeline WeaponReloadUpTimeline;

	/* The timeline for realoading down a weapon */
	FTimeline WeaponReloadDownTimeline;

	/* The new weapon to equip on EquipWeapon event */
	ABaseWeapon* NewWeaponToEquip;
};
