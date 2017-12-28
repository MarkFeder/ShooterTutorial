// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformMath.h"
#include "BaseWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_Pistol	UMETA(DisplayName = "Pistol"),
	WT_Rifle	UMETA(DisplayName = "Rifle"),
	WT_Shotgun	UMETA(DisplayName = "Shotgun")
};

UCLASS()
class SHOOTERTUTORIAL_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (ExposeOnSpawn))
	int32 IndexInBackpack;

	/* How much ammo do we have currently in the backpack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammunition")
	int32 CurrentAmmoInMag;

	/* How much ammo we can have in the magazine */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammunition")
	int32 MaxAmmoInMag = 6;

	/* How much ammo do we have currently in the backpack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammunition")
	int32 CurrentAmmoInBackpack;

	/* How much ammo we can have in the backpack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammunition")
	int32 MaxAmmoInBackpack = 30;

	/* How much time is needed to reload the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammunition")
	float ReloadTime = 2.0f;

	/* The socket this weapon will be attached to in FPP mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Socket")
	FName AttachSocketNameFPP = FName(TEXT("WeaponPoint"));

	/* The socket this weapon will be attached to in TPP mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Socket")
	FName AttachSocketNameTPP = FName(TEXT("WeaponPoint"));

	/* The type of weapon this weapon is */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType;

public:

	/* Fires this weapon */
	UFUNCTION(BlueprintNativeEvent)
	void Fire();

	/* Reloads this weapon */
	UFUNCTION(BlueprintNativeEvent)
	void Reload();

	/* Do we have enough ammo in magazine ? */
	UFUNCTION(BlueprintCallable)
	void HaveAmmoInMag(bool& HaveAmmo, bool& MagIsFull);

	/* Do we have enough ammo in backpack ? */
	UFUNCTION(BlueprintCallable)
	void HaveAmmoInBackpack(bool& HaveAmmo);

public:

	/* Sets default values for this actor's properties */
	ABaseWeapon();

protected:

	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;

public:	

	/* Called every frame */
	virtual void Tick(float DeltaTime) override;
	
};
