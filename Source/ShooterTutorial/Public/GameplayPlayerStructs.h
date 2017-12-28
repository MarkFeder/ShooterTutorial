#pragma once

#include "GameFramework/Actor.h"
#include "Engine/Texture2D.h"
#include "BaseWeapon.h"
#include "GameplayPlayerStructs.generated.h"

USTRUCT(BlueprintType)
struct FWeaponBackpackItem
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerWeapon")
	TSubclassOf<ABaseWeapon> WeaponToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerWeapon")
	UTexture2D* BackpackImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerWeapon")
	bool bIsSelected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerWeapon")
	int32 InSlot;

	FWeaponBackpackItem()
	{
		WeaponToSpawn = NULL;
		BackpackImage = NULL;
		bIsSelected = false;
		InSlot = 0;
	}
};
