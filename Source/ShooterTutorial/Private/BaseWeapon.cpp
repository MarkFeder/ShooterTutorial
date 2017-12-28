// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseWeapon.h"


void ABaseWeapon::Fire_Implementation()
{
	this->CurrentAmmoInMag -= 1;
}

void ABaseWeapon::Reload_Implementation()
{
	int32 minAmmo = FMath::Min<int32>(this->CurrentAmmoInBackpack, this->MaxAmmoInMag);
	this->CurrentAmmoInMag = minAmmo;
	this->CurrentAmmoInBackpack -= minAmmo;
}

void ABaseWeapon::HaveAmmoInMag(bool& HaveAmmo, bool& MagIsFull)
{
	HaveAmmo = this->CurrentAmmoInMag > 0;
	MagIsFull = this->CurrentAmmoInMag == this->MaxAmmoInMag;
}

void ABaseWeapon::HaveAmmoInBackpack(bool& HaveAmmo)
{
	HaveAmmo = this->CurrentAmmoInBackpack > 0;
}

ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;

	this->WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	// We are starting with full loading magazine
	this->CurrentAmmoInMag = this->MaxAmmoInMag;
	// and backpack filled with ammo
	this->CurrentAmmoInBackpack = this->MaxAmmoInBackpack;
}

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

