// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayGameMode.h"


// Set default values
AGameplayGameMode::AGameplayGameMode()
{
	this->GameStateClass = AGameplayGameState::StaticClass();
	this->PlayerControllerClass = AGameplayPlayerController::StaticClass();
	this->HUDClass = AGameplayHUD::StaticClass();
}
