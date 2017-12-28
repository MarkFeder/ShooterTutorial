// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayPlayerCharacter.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"

AGameplayPlayerCharacter::AGameplayPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the camera component
	this->Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// Attach the camera component to our capsule component
	this->Camera->SetupAttachment(Cast<USceneComponent>(GetCapsuleComponent()));
	// Allow the pawn to control camera rotation
	this->Camera->bUsePawnControlRotation = true;

	// Create the skeletal mesh component
	this->FPPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPPMesh"));
	// Attach this mesh to camera component
	FAttachmentTransformRules FPPMeshAttachmentRules(EAttachmentRule::SnapToTarget, false);
	this->FPPMesh->AttachToComponent(this->Camera, FPPMeshAttachmentRules);
}

void AGameplayPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

UShooterGameInstance* AGameplayPlayerCharacter::GetShooterGameInstance() const
{
	return Cast<UShooterGameInstance>(GetGameInstance());
}

void AGameplayPlayerCharacter::SetCurrentWeapon(ABaseWeapon *Weapon)
{
	if (Weapon == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SetCurrentWeapon:: Weapon is null or empty"))
		return;
	}

	this->CurrentWeapon = Weapon;
}

bool AGameplayPlayerCharacter::CanAddWeaponToWeaponSelected(int32& HowManyItemsSelected)
{
	if (this->BackpackWeapons.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("CanAddWeaponToWeaponSelected:: BackpackWeapons is null or empty"))
		
		HowManyItemsSelected = 0;
		return false;
	}

	int32 HowManyItems = 0;
	for (auto& Weapon : this->BackpackWeapons)
	{
		// If an item is selected, 
		// stores how many IsSelected items are there
		if (Weapon.bIsSelected)
		{
			HowManyItems++;
		}
	}

	HowManyItemsSelected = HowManyItems;
	return HowManyItemsSelected < this->SelectedInventorySpace;
}

void AGameplayPlayerCharacter::SetBackpackItemSelected(const int32& BackPackItemIndex, const bool& bIsSelected, const int32& WhichSlot)
{
	if (this->BackpackWeapons.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("SetBackpackItemSelected:: BackpackWeapons is null or empty"))
		return;
	}

	// Modify struct values inside TArray
	FWeaponBackpackItem& Weapon = this->BackpackWeapons[BackPackItemIndex];
	Weapon.bIsSelected = bIsSelected;
	Weapon.InSlot = WhichSlot;
}

void AGameplayPlayerCharacter::EquipWeapon_Implementation(ABaseWeapon* Weapon) 
{
	if ((this->CurrentWeapon == nullptr || Weapon == nullptr) && (this->CurrentWeapon == Weapon))
	{
		UE_LOG(LogTemp, Error, TEXT("EquipWeapon:: Weapon is the same as CurrentWeapon"))
		return;
	}

	// Is this dangerous?
	this->NewWeaponToEquip = Weapon;

	// Bind the function with FName OnHandleAnimPercent when the timeline advances
	FOnTimelineFloat HandleAnimPercentProgressFunction;
	HandleAnimPercentProgressFunction.BindUFunction(this, FName("OnHandleAnimPercent"));

	// Bind the function with FName OnHandleEquipWeaponFinish when the timeline finishes
	FOnTimelineEvent HandleAnimPercentEndFunction;
	HandleAnimPercentEndFunction.BindUFunction(this, FName("OnHandleEquipWeaponFinish"));
	this->EquipWeaponTimeline.SetTimelineFinishedFunc(HandleAnimPercentEndFunction);

	// Bind the function with FName HandleWeaponDownEventFunction when this event occurs
	FOnTimelineEvent HandleWeaponDownEventFunction;
	HandleWeaponDownEventFunction.BindUFunction(this, FName("OnHandleWeaponDownEvent"));
	
	// Setting up the loop status and the function that is going to fire when the timeline ticks
	this->EquipWeaponTimeline.AddInterpFloat(this->WeaponReloadUpCurve, HandleAnimPercentProgressFunction);
	this->EquipWeaponTimeline.AddEvent(0.25f, HandleWeaponDownEventFunction);
	this->EquipWeaponTimeline.SetLooping(false);

	// Start the timeline ...
	this->EquipWeaponTimeline.PlayFromStart();
}

void AGameplayPlayerCharacter::ReloadWeapon_Implementation()
{
	if (this->bIsReloading)
	{
		UE_LOG(LogTemp, Error, TEXT("ReloadWeapon:: player is already reloading"))
		
		// If we are reloading, we can't fire
		this->bCanFire = false;
		// If we have started to change weapon during reloading,
		// then we stop reloading
		this->bIsReloading = false;
		// We are starting changing the weapon but different
		// than usual
		this->bIsChangingWeapon = true;
		
		// TODO: finish ReloadWeapon when it is already reloading
		float WeaponPullDownInterp = FMath::FInterpConstantTo(this->WeaponPullDownPercent, 1.0f, FApp::GetDeltaTime(), 5.0f);
		this->WeaponPullDownPercent = WeaponPullDownInterp;

		return;
	}

	// We can't fire while reloading
	this->bIsReloading = true;
	this->bCanFire = false;

	// Bind the function with FName OnHandleWeaponReloadDown when the timeline advances
	FOnTimelineFloat WeaponReloadingDownProgressFunction;
	WeaponReloadingDownProgressFunction.BindUFunction(this, FName("OnHandleWeaponReloadDown"));

	// Bind the function with FName OnHandleWeaponReloadDownFinish when the timeline finishes
	FOnTimelineEvent WeaponReloadingDownEndFunction;
	WeaponReloadingDownEndFunction.BindUFunction(this, FName("OnHandleWeaponReloadDownFinish"));
	this->WeaponReloadDownTimeline.SetTimelineFinishedFunc(WeaponReloadingDownEndFunction);

	// Setting up the loop status and the function that is going to fire when the timeline ticks
	this->WeaponReloadDownTimeline.AddInterpFloat(this->WeaponReloadDownCurve, WeaponReloadingDownProgressFunction);
	this->WeaponReloadDownTimeline.SetLooping(false);

	// Start the timeline ...
	this->WeaponReloadDownTimeline.PlayFromStart();
}

void AGameplayPlayerCharacter::FireWeapon_Implementation()
{
	if (!this->bCanFire)
	{
		UE_LOG(LogTemp, Error, TEXT("FireWeapon:: player can't fire"))
		return;
	}

	// Do we have ammo in mag?
	bool bHaveAmmo = false;
	bool bMagIsFull = false;
	this->CurrentWeapon->HaveAmmoInMag(bHaveAmmo, bMagIsFull);

	if (bHaveAmmo)
	{
		this->CurrentWeapon->Fire();

		// Let's call dispatcher informing all subscribers
		if (this->OnCharacterFireDelegate.IsBound())
		{
			this->OnCharacterFireDelegate.Broadcast(this->CurrentWeapon->WeaponType);
		}
	}
	else
	{
		bool bHaveAmmoInBackpack = false;
		this->CurrentWeapon->HaveAmmoInBackpack(bHaveAmmoInBackpack);

		if (!bHaveAmmoInBackpack)
		{
			UE_LOG(LogTemp, Error, TEXT("FireWeapon:: there is no ammo for current weapon"))
			return;
		}

		this->ReloadWeapon();
	}
}

void AGameplayPlayerCharacter::SpawnWeaponsAndAssignToSlots()
{
	if (this->BackpackWeapons.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnWeaponsAndAssignToSlots:: BackpackWeapons is null or empty"))
		return;
	}

	for (int32 Index = 0; Index != this->BackpackWeapons.Num(); ++Index)
	{
		FWeaponBackpackItem WeaponBackpackItem = this->BackpackWeapons[Index];
		
		// Setup spawn parameter and transform rules
		FTransform NewWeaponTransform = this->GetTransform();
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);

		ABaseWeapon* SpawnedWeapon = nullptr;

		switch (WeaponBackpackItem.InSlot)
		{
			case 1:

				SpawnedWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponBackpackItem.WeaponToSpawn.Get(), NewWeaponTransform, SpawnParameters);
				SpawnedWeapon->IndexInBackpack = Index;

				this->WeaponSlot1 = SpawnedWeapon;
				this->WeaponSlot1->AttachToComponent(this->FPPMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), this->WeaponSlot1->AttachSocketNameFPP);
				// this->AttachToComponent(this->FPPMesh, AttachmentRules, this->WeaponSlot1->AttachSocketNameFPP);
				break;

			case 2:

				SpawnedWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponBackpackItem.WeaponToSpawn.Get(), NewWeaponTransform, SpawnParameters);
				SpawnedWeapon->IndexInBackpack = Index;

				this->WeaponSlot2 = SpawnedWeapon;
				this->WeaponSlot2->AttachToComponent(this->FPPMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), this->WeaponSlot2->AttachSocketNameFPP);
				// this->AttachToComponent(this->FPPMesh, AttachmentRules, this->WeaponSlot2->AttachSocketNameFPP);
				break;

			case 3:

				SpawnedWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponBackpackItem.WeaponToSpawn.Get(), NewWeaponTransform, SpawnParameters);
				SpawnedWeapon->IndexInBackpack = Index;

				this->WeaponSlot3 = SpawnedWeapon;
				this->WeaponSlot3->AttachToComponent(this->FPPMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), this->WeaponSlot3->AttachSocketNameFPP);
				// this->AttachToComponent(this->FPPMesh, AttachmentRules, this->WeaponSlot3->AttachSocketNameFPP);
				break;
		}
	}
}

void AGameplayPlayerCharacter::ShowCurrentWeapon(const ABaseWeapon* WeaponToShow)
{
	// Hide all weapons to be sure that nothing is visible
	if (this->WeaponSlot1)
		this->WeaponSlot1->SetActorHiddenInGame(true);

	if (this->WeaponSlot2)
		this->WeaponSlot2->SetActorHiddenInGame(true);
	
	if (this->WeaponSlot3)
		this->WeaponSlot3->SetActorHiddenInGame(true);

	// Do not hide weapon to show
	if (WeaponToShow == this->WeaponSlot1)
	{
		this->WeaponSlot1->SetActorHiddenInGame(false);
	}
	else if (WeaponToShow == this->WeaponSlot2)
	{
		this->WeaponSlot2->SetActorHiddenInGame(false);
	}
	else if (WeaponToShow == this->WeaponSlot3)
	{
		this->WeaponSlot3->SetActorHiddenInGame(false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ShowWeapon:: WeaponToShow is not WeaponSlot1 || WeaponSlot2 || WeaponSlot3"))
		return;
	}
}

void AGameplayPlayerCharacter::OnHandleAnimPercent(float Value)
{
	this->WeaponPullDownPercent = Value;
}

void AGameplayPlayerCharacter::OnHandleWeaponDownEvent()
{
	if (this->NewWeaponToEquip == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("OnHandleWeaponDownEvent:: NewWeaponToEquip is null or empty"))
		return;
	}

	this->SetCurrentWeapon(this->NewWeaponToEquip);
	this->ShowCurrentWeapon(this->NewWeaponToEquip);
}

void AGameplayPlayerCharacter::OnHandleEquipWeaponFinish()
{
	this->bIsChangingWeapon = false;
}

void AGameplayPlayerCharacter::OnHandleWeaponPullDownPercent(float Value)
{
	WeaponPullDownPercent = Value;
}

void AGameplayPlayerCharacter::OnFinishedHandleWeaponPullDownPercent()
{
	// Player is not changing weapon anymore
	this->bIsChangingWeapon = false;
}

void AGameplayPlayerCharacter::OnHandleWeaponReloadDown(float Value)
{
	if (!this->bIsReloading)
	{
		UE_LOG(LogTemp, Error, TEXT("OnHandleWeaponReloadDown:: GameplayPlayerCharacter is not reloading"))
		return;
	}

	this->WeaponPullDownPercent = Value;
}

void AGameplayPlayerCharacter::OnHandleWeaponReloadDownFinish()
{
	if (!this->bIsReloading)
	{
		UE_LOG(LogTemp, Error, TEXT("OnHandleWeaponReloadDownFinish:: GameplayPlayerCharacter is not reloading"))
		return;
	}

	FTimerHandle ReloadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AGameplayPlayerCharacter::OnHandleReloadTime, this->CurrentWeapon->ReloadTime, false);
}

void AGameplayPlayerCharacter::OnHandleReloadTime()
{
	if (!this->WeaponReloadUpCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("OnHandleReloadTime:: WeaponReloadUpCurve was not setup in editor"))
		return;
	}

	// Bind the function with FName OnHandleWeaponReloadUp when the timeline advances
	FOnTimelineFloat WeaponReloadingUpProgressFunction;
	WeaponReloadingUpProgressFunction.BindUFunction(this, FName("OnHandleWeaponReloadUp"));

	// Bind the function with FName OnHandleWeaponReloadUpFinish when the timeline finishes
	FOnTimelineEvent WeaponReloadingUpEndFunction;
	WeaponReloadingUpEndFunction.BindUFunction(this, FName("OnHandleWeaponReloadUpFinish"));
	this->WeaponReloadUpTimeline.SetTimelineFinishedFunc(WeaponReloadingUpEndFunction);

	// Setting up the loop status and the function that is going to fire when the timeline ticks
	this->WeaponReloadUpTimeline.AddInterpFloat(this->WeaponReloadUpCurve, WeaponReloadingUpProgressFunction);
	this->WeaponReloadUpTimeline.SetLooping(false);

	// Start the timeline ...
	this->WeaponReloadUpTimeline.PlayFromStart();
}

void AGameplayPlayerCharacter::OnHandleWeaponReloadUp(float Value)
{
	if (!this->bIsReloading)
	{
		UE_LOG(LogTemp, Error, TEXT("OnHandleWeaponReloadUp:: GameplayPlayerCharacter is not reloading"))
		return;
	}

	this->WeaponPullDownPercent = Value;
}

void AGameplayPlayerCharacter::OnHandleWeaponReloadUpFinish()
{
	if (!this->bIsReloading)
	{
		UE_LOG(LogTemp, Error, TEXT("OnHandleWeaponReloadUpFinish:: GameplayPlayerCharacter is not reloading"))
		return;
	}

	// Weapon is up, so we can add ammo now
	this->CurrentWeapon->Reload();
	this->bIsReloading = false;
	this->bCanFire = true;
}

void AGameplayPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Fires up equip weapon timeline
	if (this->EquipWeaponTimeline.IsPlaying())
	{
		this->EquipWeaponTimeline.TickTimeline(DeltaTime);
	}

	// Fires up reload weapon down timeline
	if (this->WeaponReloadDownTimeline.IsPlaying())
	{
		this->WeaponReloadDownTimeline.TickTimeline(DeltaTime);
	}

	// Fires up reload weapon up timeline
	if (this->WeaponReloadUpTimeline.IsPlaying())
	{
		this->WeaponReloadUpTimeline.TickTimeline(DeltaTime);
	}
}

void AGameplayPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
