// Fill out your copyright notice in the Description page of Project Settings.

#include "StateAndInputSystemGameModeBase.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "SamplePlayerController.h"
#include "VersusHUD.h"

void AStateAndInputSystemGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	auto firstPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	auto SamplePlayerController = Cast<ASamplePlayerController>(firstPlayerController);

	if (SamplePlayerController) 
	{
		Cast<AVersusHUD>(SamplePlayerController->GetHUD())->LoadVersusHUD(SamplePlayerController);
		SamplePlayerController->bActivated = true;
	}
		
	
}


